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

#ifndef KALDI_PYKALDI_LATGEN_WRAPPER_H_
#define KALDI_PYKALDI_LATGEN_WRAPPER_H_

/*****************
 *  C interface  *
 *****************/
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef struct {
  void *audio;
  void *mfcc;
  void *feat_input;
  void *feat_transform;
  void *feat_matrix;
  void *decodable;
  void *trans_model;
  void *amm;
  void *decoder;
  void *decode_fst;
} GmmLatgenWrapper;

GmmLatgenWrapper *new_GmmLatgenWrapper();
void del_GmmLatgenWrapper(GmmLatgenWrapper *w);
size_t GmmLatgenWrapper_Decode(void *decoder, void *decodableItf, size_t max_frames);
void GmmLatgenWrapper_FrameIn(void *audio_source, unsigned char *frame, size_t frame_len);
void GmmLatgenWrapper_GetBestPath(void *decoder, void *fst);
void GmmLatgenWrapper_GetRawLattice(void *decoder, void *fst);
void GmmLatgenWrapper_PruneFinal(void *decoder);
void GmmLatgenWrapper_Reset(GmmLatgenWrapper *w, int keep_buffer_data);
int GmmLatgenWrapper_Setup(int argc, char **argv, GmmLatgenWrapper *w);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

/*******************
 *  C++ interface  *
 *******************/
#include <string>
#include <vector>
#include "pykaldi-feat-input.h"


namespace kaldi {

struct KaldiDecoderGmmLatgenWrapperOptions  {
  /// Input sampling frequency is fixed to 16KHz
  explicit KaldiDecoderGmmLatgenWrapperOptions():kSampleFreq(16000), acoustic_scale(0.1),
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

} // namespace kaldi

#endif  // __cplusplus

#endif  // #ifdef KALDI_PYKALDI_LATGEN_WRAPPER_H_
