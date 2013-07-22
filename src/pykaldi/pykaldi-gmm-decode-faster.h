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

#ifndef KALDI_PYKALDI_GMM_DECODE_FASTER_H_
#define KALDI_PYKALDI_GMM_DECODE_FASTER_H_

#include <string>
#include <vector>
#include "feat/feature-mfcc.h"
#include "online/online-audio-source.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"
#include "pykaldi/pykaldi-audio-source.h"

/*****************
 *  C interface  *
 *****************/

typedef void CKaldiDecoderWrapper;

#ifdef __cplusplus
extern "C" {
#endif

// explicit constructor and destructor
CKaldiDecoderWrapper* new_KaldiDecoderWrapper();
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper *d);

// methods
size_t Decode(CKaldiDecoderWrapper *d);
size_t HypSize(void);
size_t FinishDecoding(CKaldiDecoderWrapper *d);
bool Finished(void);
void FrameIn(CKaldiDecoderWrapper *d, unsigned char *frame, size_t frame_len);
void PopHyp(CKaldiDecoderWrapper *d, int * word_ids, size_t size);
size_t PrepareHypothesis(CKaldiDecoderWrapper *d, int * is_full);
void Reset(CKaldiDecoderWrapper *d);
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

typedef OnlineFeInput<Mfcc> FeInput;

/** @brief A class for setting up and using Online Decoder.
 *
 *  This class provides an interface to Online Decoder.
 *  It exposes C++ as well as C API. 
 *
 *  It is absolutelly thread unsafe! */
class KaldiDecoderWrapper {
 public:
  /// Input sampling frequency is fixed to 16KHz
   KaldiDecoderWrapper():kSampleFreq_(16000), mfcc_(0), source_(0), 
    fe_input_(0), cmn_input_(0), trans_model_(0), decode_fst_(0), decoder_(0),
    feat_transform_(0), feature_matrix_(0), decodable_(0) { Reset(); }

  size_t Decode(void);

  // @brief Pass the 16 bit audio data
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void FrameIn(unsigned char *frame, size_t frame_len) {
    source_->Write(frame, frame_len);
  }

  /// May take a longer time, timeout in seconds
  size_t FinishDecoding(double timeout);

  bool Finished(void) { return (OnlineFasterDecoder::kEndFeats != decoder_->state()); }

  size_t HypSize(void) { return word_ids_.size(); }

  /// Return the buffered words and clear the buffer
  std::vector<int32> PopHyp();

  void Reset(void);

  int Setup(int argc, char **argv);

  bool UtteranceEnded() {
    OnlineFasterDecoder::DecodeState s = decoder_->state();
    return ((s  == OnlineFasterDecoder::kEndFeats) || (s == OnlineFasterDecoder::kEndUtt));
  }

  virtual ~KaldiDecoderWrapper(){ Reset(); }


 private:
  int ParseArgs(int argc, char **argv);

  bool resetted_;
  bool ready_;
  
  std::vector<int32> word_ids_;
  BaseFloat acoustic_scale_;
  int32 cmn_window_;
  const int32 kSampleFreq_;
  int32 min_cmn_window_;
  int32 left_context_;
  int32 right_context_;

  OnlineFasterDecoderOpts decoder_opts_;
  OnlineFeatureMatrixOptions feature_reading_opts_;
  DeltaFeaturesOptions delta_feat_opts_;
  std::string model_rxfilename_;
  std::string fst_rxfilename_;
  std::string word_syms_filename_; // FIXME remove it from po options
  std::string lda_mat_rspecifier_;


  Mfcc *mfcc_;
  OnlineBlockSource *source_;
  FeInput *fe_input_;
  OnlineCmnInput *cmn_input_;
  TransitionModel *trans_model_;
  fst::Fst<fst::StdArc> *decode_fst_;
  OnlineFasterDecoder *decoder_;
  OnlineFeatInputItf *feat_transform_;
  OnlineFeatureMatrix *feature_matrix_;
  OnlineDecodableDiagGmmScaled *decodable_;
  std::vector<int32> silence_phones_;
  AmDiagGmm am_gmm_;
  fst::VectorFst<LatticeArc> out_fst_;  // FIXME try it local

  KALDI_DISALLOW_COPY_AND_ASSIGN(KaldiDecoderWrapper);
};

struct KaldiDecoderWrapperOptions  {
  // FIXME not implemented. Hide the various settings from above into this class!");
};

} // namespace kaldi

#endif  // __cplusplus

#endif  // #ifndef KALDI_PYKALDI_GMM_DECODE_FASTER_H_
