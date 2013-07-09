// python-kaldi-decoding/online-wav-gmm-decode-faster.h
// -*- coding: utf-8 -*-
// FIXME rename to something like simple callback decoder

#ifndef KALDI_PYTHON_KALDI_DECODING_ONLINE_PYTHON_GMM_DECODE_FASTER_H_
#define KALDI_PYTHON_KALDI_DECODING_ONLINE_PYTHON_GMM_DECODE_FASTER_H_

#include "feat/feature-mfcc.h"
#include "online/online-audio-source.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"

#ifdef __cplusplus
extern "C" {
#endif



// FIXME rename
typedef int (*get_decoder_t)(int c, char **ar, 
      OnlineFasterDecoder * out_decoder, 
      OnlineDecodableDiagGmmScaled * out_decodable, 
      OnlineFeatInputItf *out_feat_transform, 
      fst::SymbolTable *out_word_syms,
      fst::Fst<fst::StdArc> *out_decode_fst);
// FIXME rename
typedef int (*decode_t)(OnlineFasterDecoder & decoder, 
      OnlineDecodableDiagGmmScaled & decodable, 
      fst::SymbolTable *word_syms);


int get_online_python_gmm_decode_faster(int argc, char *argv[], 
      OnlineFasterDecoder * out_decoder, 
      OnlineDecodableDiagGmmScaled * out_decodable, 
      OnlineFeatInputItf *out_feat_transform, 
      fst::SymbolTable *out_word_syms,
      fst::Fst<fst::StdArc> *out_decode_fst) {

int decode(OnlineFasterDecoder & decoder, 
      OnlineDecodableDiagGmmScaled & decodable, 
      fst::SymbolTable *word_syms);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef KALDI_PYTHON_KALDI_DECODING_ONLINE_PYTHON_GMM_DECODE_FASTER_H_
