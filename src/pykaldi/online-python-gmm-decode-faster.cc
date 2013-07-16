// -*- coding: utf-8 -*-
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
// FIXME make the OnlineFasterDecoder does not act like VAD 

#include "feat/feature-mfcc.h"
#include "online/online-audio-source.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"
#include "online/onlinebin-util.h"

#include "online-python-gmm-decode-faster.h"

/*****************
 *  C interface  *
 *****************/
// explicit constructor and destructor
CKaldiDecoderWrapper *new_KaldiDecoderWrapper() {
  return reinterpret_cast<CKaldiDecoderWrapper*>(new kaldi::KaldiDecoderWrapper());
}
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper* unallocate_pointer) {
  delete reinterpret_cast<kaldi::KaldiDecoderWrapper*>(unallocate_pointer);
}

// methods from C
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Setup(argc, argv);
} 
void Reset(CKaldiDecoderWrapper *d) {
  reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Reset();
}
void FrameIn(CKaldiDecoderWrapper *d, unsigned char *frame, size_t frame_len) {
  reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->FrameIn(frame, frame_len);
}
bool Decode(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Decode();
}
void FinishInput(CKaldiDecoderWrapper *d) {
  reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->FinishInput();
}
size_t PrepareHypothesis(CKaldiDecoderWrapper *d, int * is_full) {
  kaldi::KaldiDecoderWrapper *dp = reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
  *is_full = dp->GetHypothesis();
  return dp->last_word_ids.size();
}
void GetHypothesis(CKaldiDecoderWrapper *d, int * word_ids, size_t size) {
  kaldi::KaldiDecoderWrapper *dp = reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
  // KALDI_WARN << "DEBUG";
  for(size_t i = 0; i < size; ++i) {
    word_ids[i] = dp->last_word_ids[i];
  }
  // KALDI_WARN << "DEBUG";
}

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {


KaldiDecoderWrapper::~KaldiDecoderWrapper() {
  Reset();
}

void KaldiDecoderWrapper::Reset() {

  delete mfcc_;
  delete source_;
  delete feat_transform_;
  delete cmn_input_;
  delete trans_model_;
  delete decode_fst_;
  delete decoder_;
  delete decodable_;
  silence_phones_.clear();
  word_syms_filename_.clear(); // FIXME remove it from options
  last_word_ids.clear();

  mfcc_ = 0;
  source_ = 0;
  feat_transform_ = 0;
  cmn_input_ = 0;
  trans_model_ = 0;
  decode_fst_ = 0;
  decoder_ = 0;
  feature_matrix_ = 0;
  decodable_ = 0;

  // Up to delta-delta derivative features are calculated unless LDA is used
  // default values: order & window
  delta_feat_opts_ = DeltaFeaturesOptions(2, 2); 

  acoustic_scale_ = 0.1;
  cmn_window_ = 600; min_cmn_window_ = 100;
  right_context_ = 4; left_context_ = 4;

  decoder_opts_ = OnlineFasterDecoderOpts();
  feature_reading_opts_ = OnlineFeatureMatrixOptions();
  model_rxfilename_.clear();
  fst_rxfilename_.clear();
  lda_mat_rspecifier_.clear();

  resetted_ = true; ready_ = false;
  // KALDI_WARN << "DEBUG";
} // Reset ()

int KaldiDecoderWrapper::Setup(int argc, char **argv) {
  ready_ = false; resetted_ = false;
  try {
    // KALDI_WARN << "DEBUG";
    if (ParseArgs(argc, argv) != 0) {
      Reset(); 
      return 1;
    }

    trans_model_ = new TransitionModel();
    {
      bool binary;
      Input ki(model_rxfilename_, &binary);
      trans_model_->Read(ki.Stream(), binary);
      am_gmm_.Read(ki.Stream(), binary);
    }

    // KALDI_WARN << "DEBUG";
    decode_fst_ = ReadDecodeGraph(fst_rxfilename_);
    decoder_ = new OnlineFasterDecoder(*decode_fst_, decoder_opts_,
                                    silence_phones_, *trans_model_);

    // Fixed 16 bit audio
    source_ = new OnlineBlockSource(); 

    // We are not properly registering/exposing MFCC and frame extraction options,
    // because there are parts of the online decoding code, where some of these
    // options are hardwired(ToDo: we should fix this at some point)
    MfccOptions mfcc_opts;
    mfcc_opts.use_energy = false;
    int32 frame_length = mfcc_opts.frame_opts.frame_length_ms = 25;
    int32 frame_shift = mfcc_opts.frame_opts.frame_shift_ms = 10;
    mfcc_ = new Mfcc(mfcc_opts);

    fe_input_ = new FeInput(source_, mfcc_,
                               frame_length * (kSampleFreq_ / 1000),
                               frame_shift * (kSampleFreq_ / 1000));
    cmn_input_ = new OnlineCmnInput(fe_input_, cmn_window_, min_cmn_window_);

    // KALDI_WARN << "DEBUG";
    if (lda_mat_rspecifier_ != "") {
      bool binary_in;
      Matrix<BaseFloat> lda_transform; 
      Input ki(lda_mat_rspecifier_, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
      // KALDI_WARN << "DEBUG";
      // lda_transform is copied to OnlineLdaInput
      feat_transform_ = new OnlineLdaInput(cmn_input_, 
                                lda_transform,
                                left_context_, right_context_);
      // KALDI_WARN << "DEBUG";
    } else {
      // Note from Dan: keeping the next statement for back-compatibility,
      // but I don't think this is really the right way to set the window-size
      // in the delta computation: it should be a separate config.
      delta_feat_opts_.window = left_context_ / 2;
      // KALDI_WARN << "DEBUG";
      feat_transform_ = new OnlineDeltaInput(delta_feat_opts_, cmn_input_);
      // KALDI_WARN << "DEBUG";
    }
    // KALDI_WARN << "DEBUG";

    // feature_reading_opts_ contains timeout, batch size.
    feature_matrix_ = new OnlineFeatureMatrix(feature_reading_opts_,
                                       feat_transform_);
    decodable_ = new OnlineDecodableDiagGmmScaled(am_gmm_, 
                                            *trans_model_, 
                                            acoustic_scale_, feature_matrix_);
    resetted_ = false; ready_ = true;
    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    Reset();
    return 2;
  }
} // KaldiDecoderWrapper::Setup()

void KaldiDecoderWrapper::FrameIn(unsigned char *frame, size_t frame_len) {
  source_->Write(frame, frame_len);
}

bool KaldiDecoderWrapper::Decode(void) {
  OnlineFasterDecoder::DecodeState state = decoder_->Decode(decodable_);
  // KALDI_WARN << "DEBUG";
  return  state != OnlineFasterDecoder::kEndFeats;
}

void KaldiDecoderWrapper::FinishInput(void) {
  // FIXME maybe also grab the last hypothesis here
  source_->NoMoreInput();
}

/// Return bool: True for full hypothesis, False for partial. 
/// For empty hypothesis also returns True.
/// Throw away previously decoded buffered hypothesis.
bool KaldiDecoderWrapper::GetHypothesis() {
  last_word_ids.clear();
  // fst::VectorFst<LatticeArc> out_fst;  // FIXME try it local
  if (UtteranceEnded()) {
    // get the last chunk
    decoder_->FinishTraceBack(&out_fst_);
    fst::GetLinearSymbolSequence(out_fst_,
                                 static_cast<vector<int32> *>(0),
                                 &last_word_ids,
                                 static_cast<LatticeArc::Weight*>(0));

    // // TODO DEBUG
    // decoder_->GetBestPath(&out_fst_);
    // std::vector<int32> tids;
    // fst::GetLinearSymbolSequence(out_fst_,
    //                              &tids,
    //                              &last_word_ids,
    //                              static_cast<LatticeArc::Weight*>(0));
    // std::stringstream res_key;
    // res_key << "test.wav_" << "<?>" << '-' << decoder_->frame();
    // if (!last_word_ids.empty())
    //   words_writer_.Write(res_key.str(), last_word_ids);
    // alignment_writer_.Write(res_key.str(), tids);

  } else {
    // get the hypothesis from currently active state
    if (decoder_->PartialTraceback(&out_fst_)) {
      fst::GetLinearSymbolSequence(out_fst_,
                                   static_cast<vector<int32> *>(0),
                                   &last_word_ids,
                                   static_cast<LatticeArc::Weight*>(0));
    }
  }
  // empty hypothesis is full hypothesis (not partial one)
  return (UtteranceEnded() || last_word_ids.size() == 0) ;
} 

bool KaldiDecoderWrapper::GetHypothesis(std::vector<int32> & word_ids) {
  bool result = GetHypothesis();
  word_ids = this->last_word_ids;
  // KALDI_WARN << "DEBUG";
  return result;
}

int KaldiDecoderWrapper::ParseArgs(int argc, char ** argv) {
  try {

    ParseOptions po("Utterance segmentation is done on-the-fly.\n"
      "Feature splicing/LDA transform is used, if the optional(last) " 
      "argument is given.\n"
      "Otherwise delta/delta-delta(2-nd order) features are produced.\n\n"
      "Usage: online-gmm-Decode-faster [options] <model-in>"
      "<fst-in> <word-symbol-table> <silence-phones> [<lda-matrix-in>]\n\n"
      "Example: online-gmm-Decode-faster --rt-min=0.3 --rt-max=0.5 "
      "--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 "
      "model HCLG.fst words.txt '1:2:3:4:5' lda-matrix");

    decoder_opts_.Register(&po, true);
    feature_reading_opts_.Register(&po);
    
    po.Register("left-context", &left_context_, "Number of frames of left context");
    po.Register("right-context", &right_context_, "Number of frames of right context");
    po.Register("acoustic-scale", &acoustic_scale_,
                "Scaling factor for acoustic likelihoods");
    po.Register("cmn-window", &cmn_window_,
        "Number of feat. vectors used in the running average CMN calculation");
    po.Register("min-cmn-window", &min_cmn_window_,
                "Minumum CMN window used at start of decoding (adds "
                "latency only at start)");

    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return 1;
    }
    if (po.NumArgs() == 4)
      if (left_context_ % delta_feat_opts_.order != 0 || 
          left_context_ != right_context_)
        KALDI_ERR << "Invalid left/right context parameters!";

    int32 window_size = right_context_ + left_context_ + 1;
    decoder_opts_.batch_size = std::max(decoder_opts_.batch_size, window_size);

    model_rxfilename_ = po.GetArg(1);
    fst_rxfilename_ = po.GetArg(2);
    word_syms_filename_ = po.GetArg(3);
    std::string silence_phones_str = po.GetArg(4);
    lda_mat_rspecifier_ = po.GetOptArg(5);

    if (!SplitStringToIntegers(silence_phones_str, ":", false, &silence_phones_))
        KALDI_ERR << "Invalid silence-phones string " << silence_phones_str;
    if (this->silence_phones_.empty())
        KALDI_ERR << "No silence phones given!";

    // KALDI_WARN << "DEBUG";
    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }
}  // KaldiDecoderWrapper::ParseArgs()

bool KaldiDecoderWrapper::UtteranceEnded() {
  // FIXME I should detect probably this myself from the Dialog System
  // KALDI_WARN << "DEBUG";
  return decoder_->state() & (OnlineFasterDecoder::kEndFeats | 
      OnlineFasterDecoder::kEndUtt);
}

} // namespace kaldi
