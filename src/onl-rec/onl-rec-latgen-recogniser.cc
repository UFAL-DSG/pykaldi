// -*- coding: utf-8 -*-
/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
 *               2012-2013  Vassil Panayotov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
 * WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
 * MERCHANTABLITY OR NON-INFRINGEMENT.
 * See the Apache 2 License for the specific language governing permissions and
 * limitations under the License. */

#include <fst/arc-map.h>
#include <fst/arc.h>
#include "fstext/fstext-lib.h"
#include "feat/feature-mfcc.h"
#include "feat/feature-functions.h"
#include "feat/online-feature.h"
#include "util/stl-utils.h"
#include "decoder/lattice-faster-online-decoder.h"
#include "online2/online-gmm-decodable.h"
#include "online2/onlinebin-util.h"
#include "onl-rec/onl-rec-utils.h"
#include "onl-rec/onl-rec-latgen-recogniser.h"

#ifdef DEBUG
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#endif


namespace kaldi {

struct OnlineLatgenRecogniserConfig {

  explicit OnlineLatgenRecogniserConfig():acoustic_scale(0.1), bits_per_sample(16) {}

  void Register(OptionsItf *po) {
    mfcc_opts.Register(po);
    splice_opts.Register(po);
    // delta_feat_opts.Register(po);
    faster_decoder_opts.Register(po);
    po->Register("bits-per-sample", &bits_per_sample,
                "Number of bits used for one sample in input waveform e.g. 8, 16.");
    po->Register("acoustic-scale", &acoustic_scale,
                "Scaling factor for acoustic likelihoods for forward decoding");
  }
  BaseFloat acoustic_scale;
  int32 bits_per_sample;

  std::string model_rxfilename;
  std::string fst_rxfilename;
  std::string silence_phones;
  std::string lda_mat_rspecifier;

  MfccOptions mfcc_opts;
  OnlineSpliceOptions splice_opts;
  // DeltaFeaturesOptions delta_feat_opts; TODO add support again
  LatticeFasterDecoderConfig faster_decoder_opts;
};



void OnlineLatgenRecogniser::Deallocate() {
  initialized_ = false;

  silence_phones_.clear();
  delete mfcc_; mfcc_ = NULL;
  delete splice_; splice_ = NULL;
  delete transform_; transform_ = NULL;
  delete decodable_; decodable_ = NULL;
  delete trans_model_; trans_model_ = NULL;
  delete am_; am_ = NULL;
  delete decoder_; decoder_ = NULL;
  delete hclg_; hclg_ = NULL;
  delete  config_; config_ = NULL;
  delete lda_mat_; lda_mat_ = NULL;
}

OnlineLatgenRecogniser::~OnlineLatgenRecogniser() {
  Deallocate();
}

size_t OnlineLatgenRecogniser::Decode(size_t max_frames) {
  if (! initialized_)
    return 0;
  size_t decoded = decoder_->NumFramesDecoded();
  decoder_->AdvanceDecoding(decodable_, max_frames);
  return decoder_->NumFramesDecoded() - decoded;
}


void OnlineLatgenRecogniser::FrameIn(unsigned char *frame, size_t frame_len) {
  if (! initialized_)
    return;
  Vector<BaseFloat> waveform(frame_len);
  for(size_t i = 0; i < frame_len; ++i) {
    switch(config_->bits_per_sample) {
      case 8:
        {
          waveform(i) = (*frame);
          frame++;
          break;
        }
      case 16:
        {
          int16 k = *reinterpret_cast<uint16*>(frame);
#ifdef __BIG_ENDDIAN__
          KALDI_SWAP2(k);
#endif
          waveform(i) = k;
          frame += 2;
          break;
        }
      default:
        KALDI_ERR << "Unsupported bits ber sample (implement yourself): "
          << config_->bits_per_sample;
    }
  }
  mfcc_->AcceptWaveform(config_->mfcc_opts.frame_opts.samp_freq, waveform);
}


bool OnlineLatgenRecogniser::GetBestPath(std::vector<int> *out_ids, BaseFloat *prob) {
  *prob = -1.0;  // default value for failures
  if (! initialized_)
    return false;
  Lattice lat;
  bool ok = decoder_->GetBestPath(&lat);
  LatticeWeight weight;
  fst::GetLinearSymbolSequence(lat,
                               static_cast<vector<int32> *>(0),
                               out_ids,
                               &weight);

    *prob = weight.Value1() + weight.Value2();
  return ok;
}

void OnlineLatgenRecogniser::FinalizeDecoding() {
  decoder_->FinalizeDecoding();
}


bool OnlineLatgenRecogniser::GetLattice(fst::VectorFst<fst::LogArc> *fst_out, 
                                  double *tot_lik, bool end_of_utterance) {
  if (! initialized_)
    return false;

  CompactLattice lat;
  Lattice raw_lat;
  if (decoder_->NumFramesDecoded() == 0)
    KALDI_ERR << "You cannot get a lattice if you decoded no frames.";
  bool ok = decoder_->GetRawLattice(&raw_lat, end_of_utterance);

  if (!config_->faster_decoder_opts.determinize_lattice)
    KALDI_ERR << "--determinize-lattice=false option is not supported at the moment";

  BaseFloat lat_beam = config_->faster_decoder_opts.lattice_beam;
  DeterminizeLatticePhonePrunedWrapper(
      *trans_model_, &raw_lat, lat_beam, &lat, config_->faster_decoder_opts.det_opts);

  *tot_lik = CompactLatticeToWordsPost(lat, fst_out);

  return ok;
}


void OnlineLatgenRecogniser::ResetPipeline() {
    delete mfcc_;
    mfcc_ = new OnlineMfcc(config_->mfcc_opts);
    delete splice_;
    splice_ = new OnlineSpliceFrames(config_->splice_opts, mfcc_);
    delete transform_;
    transform_ = new OnlineTransform(*lda_mat_, splice_);
    delete decodable_;
    decodable_ = new DecodableDiagGmmScaledOnline(*am_,
                                            *trans_model_,
                                            config_->acoustic_scale, transform_);
}

void OnlineLatgenRecogniser::Reset(bool reset_pipeline) {
  if (! initialized_)
    return;
  if(reset_pipeline)
    ResetPipeline();
  decoder_->InitDecoding();
}


bool OnlineLatgenRecogniser::Setup(int argc, char **argv) {
  initialized_ = false;
  try {
    if (config_ == NULL)
      config_ = new OnlineLatgenRecogniserConfig();  

    // Parsing options
    ParseOptions po("Utterance segmentation is done on-the-fly.\n"
      "The delta/delta-delta(2-nd order) features are produced.\n\n"
      "Usage: decoder-binary-name [options] <model-in>"
      "<fst-in> <silence-phones> \n\n"
      "Example: decoder-binary-name --max-active=4000 --beam=12.0 "
      "--acoustic-scale=0.0769 model HCLG.fst words.txt '1:2:3:4:5'");

    config_->Register(&po);

    po.Read(argc, argv);
    if (po.NumArgs() != 4) {
      po.PrintUsage();
      return false;
    }
    config_->model_rxfilename = po.GetArg(1);
    config_->fst_rxfilename = po.GetArg(2);
    config_->silence_phones = (po.GetArg(3));
    config_->lda_mat_rspecifier = po.GetOptArg(4);

    // Setting up components
    trans_model_ = new TransitionModel();
    am_ = new AmDiagGmm();
    {
      bool binary;
      Input ki(config_->model_rxfilename, &binary);
      trans_model_->Read(ki.Stream(), binary);
      am_->Read(ki.Stream(), binary);
    }

    hclg_ = ReadDecodeGraph(config_->fst_rxfilename);
    decoder_ = new LatticeFasterOnlineDecoder(*hclg_, 
                                              config_->faster_decoder_opts);

    if (!SplitStringToIntegers(config_->silence_phones, ":", false, 
                                &silence_phones_)) {
      KALDI_WARN << "Bad silence-phone argument '" << config_->silence_phones << "'" ;
      return false;
    } else 
      SortAndUniq(&silence_phones_);
   

    bool binary_in;
    Input ki(config_->lda_mat_rspecifier, &binary_in);
    delete lda_mat_;
    lda_mat_ = new Matrix<BaseFloat>();
    lda_mat_->Read(ki.Stream(), binary_in);
    KALDI_VLOG(1) << "LDA will be used for decoding" << std::endl;

    // put the peaces of pipeline setup above together
    ResetPipeline(); 
    // No beter place to do elsewhere
    decoder_->InitDecoding();
     
  } catch(const std::exception& e) {
    Deallocate();
    // throw e;
    std::cerr << e.what() << std::endl;
    return false;
  }
  initialized_ = true;
  return initialized_;
}

} // namespace kaldi
