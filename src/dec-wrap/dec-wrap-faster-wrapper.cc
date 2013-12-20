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

#include "feat/feature-mfcc.h"
#include "dec-wrap/dec-wrap-utils.h"
#include "dec-wrap/dec-wrap-audio-source.h"
#include "dec-wrap/dec-wrap-faster-wrapper.h"
#include "dec-wrap/dec-wrap-feat-input.h"
#include "dec-wrap/dec-wrap-decodable.h"

/*****************
 *  C interface  *
 *****************/
// explicit constructor and destructor
CKaldiDecoderWrapper *new_KaldiDecoderWrapper(void) {
  kaldi::KaldiDecoderWrapperOptions opts;
  return reinterpret_cast<CKaldiDecoderWrapper*>(new kaldi::KaldiDecoderWrapper(opts));
}
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper* d) {
  delete reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
}

// methods from C
size_t Decode(CKaldiDecoderWrapper *d, int force_utt_end) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Decode(force_utt_end);
}
size_t HypSize(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->HypSize();
}
void FrameIn(CKaldiDecoderWrapper *d, unsigned char *frame, size_t frame_len) {
  reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->FrameIn(frame, frame_len);
}
void PopHyp(CKaldiDecoderWrapper *d, int * word_ids, size_t size) {
  kaldi::KaldiDecoderWrapper *dp = reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
  std::vector<int32> tmp = dp->PopHyp();
  KALDI_ASSERT(size <= tmp.size());
  std::copy(tmp.begin(), tmp.begin()+size, word_ids);

}
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Setup(argc, argv);
}

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

size_t KaldiDecoderWrapper::Decode(bool force_utt_end) {
  KALDI_VLOG(2) << " audio source size: " << source_->BufferSize();

  size_t frames_processed = decoder_->Decode(decodable_);

  if (frames_processed > 0 ) {
    fst::VectorFst<LatticeArc> out_fst;
    std::vector<int32> new_word_ids;
    if (UtteranceEnded() || force_utt_end) {
      // get the last chunk
      decoder_->FinishTraceBack(&out_fst);
      fst::GetLinearSymbolSequence(out_fst,
                                   static_cast<vector<int32> *>(0),
                                   &new_word_ids,
                                   static_cast<LatticeArc::Weight*>(0));
    } else {
      // get the hypothesis from currently active state
      if (decoder_->PartialTraceback(&out_fst)) {
        fst::GetLinearSymbolSequence(out_fst,
                                     static_cast<vector<int32> *>(0),
                                     &new_word_ids,
                                     static_cast<LatticeArc::Weight*>(0));
      }
    }
    // append the new ids to buffer
    word_ids_.insert(word_ids_.end(), new_word_ids.begin(), new_word_ids.end());
  }

  return word_ids_.size();
}


// Looks unficcient but: c++11 move semantics
// and RVO: http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
// justified it. It has nicer interface.
std::vector<int32> KaldiDecoderWrapper::PopHyp(void) {
  std::vector<int32> tmp;
  std::swap(word_ids_, tmp); // clear the word_ids_
  KALDI_VLOG(2) << "tmp.size() " << tmp.size();
  return tmp;
}

int KaldiDecoderWrapper::Setup(int argc, char **argv) {
  try {
    PykaldiFeatureMatrixOptions feature_reading_opts_;
    MfccOptions mfcc_opts_;
    PykaldiFasterDecoderOpts decoder_opts_;
    DeltaFeaturesOptions delta_feat_opts_;

    mfcc_opts_.use_energy = false;
    mfcc_opts_.frame_opts.frame_length_ms = 25;
    mfcc_opts_.frame_opts.frame_shift_ms = 10;

    // Parsing options
    ParseOptions po("Utterance segmentation is done on-the-fly.\n"
      "The delta/delta-delta(2-nd order) features are produced.\n\n"
      "Usage: decoder-binary-name [options] <model-in>"
      "<fst-in> <word-symbol-table> <silence-phones> \n\n"
      "Example: decoder-binary-name --max-active=4000 --beam=12.0 "
      "--acoustic-scale=0.0769 model HCLG.fst words.txt '1:2:3:4:5'");

    opts_.Register(&po);
    mfcc_opts_.Register(&po);
    decoder_opts_.Register(&po, true);
    feature_reading_opts_.Register(&po);
    delta_feat_opts_.Register(&po);

    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return 1;
    }
    if (po.NumArgs() == 4)
      if (opts_.left_context % delta_feat_opts_.order != 0 ||
          opts_.left_context != opts_.right_context)
        KALDI_ERR << "Invalid left/right context parameters!";

    opts_.model_rxfilename = po.GetArg(1);
    opts_.fst_rxfilename = po.GetArg(2);
    opts_.word_syms_filename = po.GetArg(3);

    opts_.silence_phones = phones_to_vector(po.GetArg(4));
    opts_.lda_mat_rspecifier = po.GetOptArg(5);

    // Setting up components
    trans_model_ = new TransitionModel();
    {
      bool binary;
      Input ki(opts_.model_rxfilename, &binary);
      trans_model_->Read(ki.Stream(), binary);
      am_gmm_.Read(ki.Stream(), binary);
    }

    decode_fst_ = ReadDecodeGraph(opts_.fst_rxfilename);
    decoder_ = new PykaldiFasterDecoder(*decode_fst_, decoder_opts_,
                                    opts_.silence_phones, *trans_model_);

    PykaldiBuffSourceOptions au_opts;  // Fixed 16 bit audio
    source_ = new PykaldiBuffSource(au_opts);

    mfcc_ = new Mfcc(mfcc_opts_);
    int32 frame_length = mfcc_opts_.frame_opts.frame_length_ms;
    int32 frame_shift = mfcc_opts_.frame_opts.frame_shift_ms;
    fe_input_ = new PykaldiFeInput<Mfcc>(source_, mfcc_,
                               frame_length * (opts_.kSampleFreq / 1000),
                               frame_shift * (opts_.kSampleFreq / 1000));

    if (opts_.lda_mat_rspecifier != "") {
      bool binary_in;
      Matrix<BaseFloat> lda_transform;
      Input ki(opts_.lda_mat_rspecifier, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
      // lda_transform is copied to PykaldiLdaInput
      feat_transform_ = new PykaldiLdaInput(fe_input_,
                                lda_transform,
                                opts_.left_context, opts_.right_context);
    } else {
      feat_transform_ = new PykaldiDeltaInput(delta_feat_opts_, fe_input_);
    }

    feature_matrix_ = new PykaldiFeatureMatrix(feature_reading_opts_,
                                       feat_transform_);
    decodable_ = new PykaldiDecodableDiagGmmScaled(am_gmm_,
                                            *trans_model_,
                                            opts_.acoustic_scale, feature_matrix_);
    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return 1;
  }
} // KaldiDecoderWrapper::Setup()


} // namespace kaldi
