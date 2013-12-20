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

#ifndef KALDI_DEC_WRAP_GMM_DECODE_FASTER_H_
#define KALDI_DEC_WRAP_GMM_DECODE_FASTER_H_

/*****************
 *  C interface  *
 *****************/
#include <stdio.h>

typedef void CKaldiDecoderWrapper;

#ifdef __cplusplus
extern "C" {
#endif

// explicit constructor and destructor
CKaldiDecoderWrapper* new_KaldiDecoderWrapper(void);
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper *d);

// methods
size_t Decode(CKaldiDecoderWrapper *d, int force_utt_end);
size_t HypSize(CKaldiDecoderWrapper *d);
void FrameIn(CKaldiDecoderWrapper *d, unsigned char *frame, size_t frame_len);
void PopHyp(CKaldiDecoderWrapper *d, int * word_ids, size_t size);
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv);
// FIXME todo ClearBuffers()  : it will reset the buffers in pipeline

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

/*******************
 *  C++ interface  *
 *******************/
#include <string>
#include <vector>
#include "feat/feature-mfcc.h"
#include "dec-wrap/dec-wrap-faster-decoder.h"
#include "dec-wrap/dec-wrap-feat-input.h"
#include "dec-wrap/dec-wrap-decodable.h"
#include "dec-wrap/dec-wrap-audio-source.h"


namespace kaldi {

struct KaldiDecoderWrapperOptions  {
  /// Input sampling frequency is fixed to 16KHz
  explicit KaldiDecoderWrapperOptions():kSampleFreq(16000), acoustic_scale(0.1),
  left_context(4), right_context(4)
  {}
  int32 kSampleFreq;
  BaseFloat acoustic_scale;
  int32 left_context;
  int32 right_context;
  std::string model_rxfilename;
  std::string fst_rxfilename;
  std::string word_syms_filename; // FIXME remove it from po options
  std::string lda_mat_rspecifier;
  std::vector<int32> silence_phones;
  void Register(OptionsItf *po) {
    po->Register("left-context", &left_context, "Number of frames of left context");
    po->Register("right-context", &right_context, "Number of frames of right context");
    po->Register("acoustic-scale", &acoustic_scale,
                "Scaling factor for acoustic likelihoods");
  }
};


/** @brief A class for setting up and using Pykaldi Decoder.
 *
 *  This class provides an interface to Pykaldi Decoder.
 *  It exposes C++ as well as C API.
 *
 *  It is absolutelly thread unsafe! */
class KaldiDecoderWrapper {
 public:
   // The default parameters can be overriden in Setup function
   KaldiDecoderWrapper(const KaldiDecoderWrapperOptions &opts):opts_(opts),
    mfcc_(0), source_(0), fe_input_(0), trans_model_(0), decode_fst_(0),
    decoder_(0), feat_transform_(0), feature_matrix_(0), decodable_(0) { }

  /// @brief Do forward decoding and partial backward decoding if it is in
  /// the middle of utterance. At the end of utterance it performs full backward decoding.
  /// The decoded output is buffered word_ids variable
  /// @param force_utt_end [in] The data which are in audio_source are last data for utterance
  /// and so we enforce full backward decoding
  size_t Decode(bool force_utt_end=false);

  // @brief Pass the 16 bit audio data
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void FrameIn(unsigned char *frame, size_t frame_len)
    { source_->Write(frame, frame_len); }

  size_t HypSize(void) { return word_ids_.size(); }

  /// Return the buffered words and clear the buffer
  std::vector<int32> PopHyp();

  // Change the default values by specifying parameters like command line args
  int Setup(int argc, char **argv);

  bool UtteranceEnded() { return decoder_->EndOfUtterance(); }

  virtual ~KaldiDecoderWrapper(){
    delete mfcc_; delete source_; delete feat_transform_; delete trans_model_;
    delete decode_fst_; delete decoder_; delete decodable_;
  }

 private:
  KaldiDecoderWrapperOptions opts_;
  std::vector<int32> word_ids_;

  Mfcc *mfcc_;
  PykaldiBuffSource *source_;
  PykaldiFeInput<Mfcc> *fe_input_;
  TransitionModel *trans_model_;
  fst::Fst<fst::StdArc> *decode_fst_;
  PykaldiFasterDecoder *decoder_;
  PykaldiFeatInputItf *feat_transform_;
  PykaldiFeatureMatrix *feature_matrix_;
  PykaldiDecodableDiagGmmScaled *decodable_;
  AmDiagGmm am_gmm_;

  KALDI_DISALLOW_COPY_AND_ASSIGN(KaldiDecoderWrapper);
};


} // namespace kaldi

#endif  // __cplusplus

#endif  // #ifndef KALDI_DEC_WRAP_GMM_DECODE_FASTER_H_
