// pykaldi/pykaldi-lattice-faster-decoder.h
/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
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

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "gmm/am-diag-gmm.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/lattice-faster-decoder.h"
#include "gmm/decodable-am-diag-gmm.h"
#include "util/timer.h"
#include "feat/feature-functions.h"  // feature reversal
#include "pykaldi/pykaldi-lattice-faster-decoder.h"

/*****************
 *  C interface  *
 *****************/
// explicit constructor and destructor

CWrapperLatFastDecoder* new_WrapperLatFastDecoder(void) {
  return reinterpret_cast<CWrapperLatFastDecoder*>(new kaldi::WrapperLatFastDecoder());
}
void del_WrapperLatFastDecoder(CWrapperLatFastDecoder *d) {
  delete reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d);
}

// methods
// TODO full list
bool WLFD_Decode(CWrapperLatFastDecoder *d) {
  return reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d)->Decode();
}
bool WLFD_Finished(CWrapperLatFastDecoder *d) {
  return reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d)->Finished();
}
void WLFD_FrameIn(CWrapperLatFastDecoder *d, unsigned char *frame, size_t frame_len) {
  reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d)->FrameIn(frame, frame_len);
}
void WLFD_Reset(CWrapperLatFastDecoder *d) {
  reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d)->Reset();
}
int WLFD_Setup(CWrapperLatFastDecoder *d, int argc, char **argv) {
  return reinterpret_cast<kaldi::WrapperLatFastDecoder*>(d)->Setup(argc, argv);
}

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

bool WrapperLatFastDecoder::Decode(void) {

  if (feature_reader_.Done())
    return false; 
  std::string utt = feature_reader_.Key();
  Matrix<BaseFloat> features (feature_reader_.Value());
  feature_reader_.FreeCurrent();
  if (features.NumRows() == 0) {
    KALDI_WARN << "Zero-length utterance: " << utt;
    num_err++;
    return false;
  }
  
  DecodableAmDiagGmmScaled gmm_decodable(am_gmm_, *trans_model_, features,
                                         opts_.acoustic_scale);

  double like;
  LatticeWriter lattice_writer;
  if (DecodeUtteranceLatticeFaster(
          *decoder_, gmm_decodable, word_syms_, utt, opts_.acoustic_scale,
          true, opts_.allow_partial, &alignment_writer, &words_writer,
          &compact_lattice_writer, &lattice_writer, &like)) {
    tot_like += like;
    frame_count += features.NumRows();
    num_done++;
  } else num_err++;

  return true;
}

void WrapperLatFastDecoder::Reset(void) {
  
  // FIXME does not reset lattice_writer, words_writer, etc
  // because I will remove them shortly
  // they are just for debuggin purposes

  delete word_syms_;
  delete trans_model_;
  delete decode_fst_;
  delete source_;
  delete decoder_;

  word_syms_ = 0;
  trans_model_ = 0;
  decode_fst_ = 0;
  source_ = 0;
  decoder_ = 0;

  frame_count = num_done = num_err = 0;
  tot_like = 0.0;

  opts_ = WrapperLatFastDecoderOptions();
  config_ = LatticeFasterDecoderConfig();
}

int WrapperLatFastDecoder::ParseArgs(int argc, char **argv) {
  try {
    ParseOptions po("Generate lattices using GMM-based model.\n"
        "Usage: gmm-latgen-faster [options] model-in (fst-in|fsts-rspecifier) "
        "features-rspecifier lattice-wspecifier "
        "[ words-wspecifier [alignments-wspecifier] ]\n");

    config_.Register(&po);
    opts_.Register(&po);

    po.Read(argc, argv);

    if (po.NumArgs() < 4 || po.NumArgs() > 6) {
      po.PrintUsage();
      return 1;
    }

    opts_.model_in_filename = po.GetArg(1);
    opts_.fst_in_str = po.GetArg(2);
    opts_.feature_rspecifier = po.GetArg(3);
    opts_.lattice_wspecifier = po.GetArg(4);
    opts_.words_wspecifier = po.GetOptArg(5);
    opts_.alignment_wspecifier = po.GetOptArg(6);

    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }
} // end ParseArgs()


int WrapperLatFastDecoder::Setup(int argc, char ** argv) {
  try {
    if (ParseArgs(argc, argv) != 0) {
      Reset(); 
      return 1;
    }

    trans_model_ = new TransitionModel();
    {
      bool binary;
      Input ki(opts_.model_in_filename, &binary);
      trans_model_->Read(ki.Stream(), binary);
      am_gmm_.Read(ki.Stream(), binary);
    }


    if(!compact_lattice_writer.Open(opts_.lattice_wspecifier))
      KALDI_ERR << "Could not open table for writing lattices: "
                << opts_.lattice_wspecifier;

    words_writer = Int32VectorWriter(opts_.words_wspecifier);
    alignment_writer = Int32VectorWriter(opts_.alignment_wspecifier);

    if (opts_.word_syms_filename != "") 
      if (!(word_syms_ = fst::SymbolTable::ReadText(opts_.word_syms_filename)))
        KALDI_ERR << "Could not read symbol table from file "
                   << opts_.word_syms_filename;

      feature_reader_ = SequentialBaseFloatMatrixReader(opts_.feature_rspecifier);

      // Input FST is just one FST, not a table of FSTs.
      decode_fst_ = fst::ReadFstKaldi(opts_.fst_in_str);
      decoder_= new LatticeFasterDecoder(*decode_fst_, config_);

  } catch(const std::exception& e) {
    std::cerr << e.what();
    Reset();
    return 2;
  }
  return 0;
} // Setup()


} // end namespace kaldi.

