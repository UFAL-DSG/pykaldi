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

/*******************
 *  C++ interface  *
 *******************/


// S source class type, E features Extractor MFCC or PLP 
class KaldiDecoderWrapper {
 public:
  // methods designed also for calls from C
  KaldiDecoderWrapper(void);  
  void build(int argc, char * argv[]);
  void set_up(void); 
  void disconnect(void);
  void decode(char * out_str);
  virtual ~KaldiDecoderWrapper;

  // methods for C++ interface only
  void set_feat_input(OnlineFeatInputItf * feat_input);
  void set_decode_fst(fst::Fst<fst::StdArc> *decode_fst);
  void set_word_symbols(fst::SymbolTable *word_syms);
  void set_decoder(kaldi::OnlineFasterDecoder * decoder); 
  void set_decodable(kaldi::OnlineDecodableDiagGmmScaled * decodable);

  KALDI_DISALLOW_COPY_AND_ASSIGN(KaldiDecoderWrapper);
};

#endif  // __cplusplus



/*****************
 *  C interface  *
 *****************/

typedef void* CKaldiDecoderWrapper;

#ifdef __cplusplus
extern "C" {
#endif

// explicit constructor and destructor
CKaldiDecoderWrapper new_KaldiDecoderWrapper();
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper unallocate_pointer);

// methods
void build(CKaldiDecoderWrapper d, int argc, char * argv[]); 
void set_up(CKaldiDecoderWrapper d); 
void disconnect(CKaldiDecoderWrapper d); 
void decode(CKaldiDecoderWrapper d, char * out_str); 

#ifdef __cplusplus
}
#endif



#endif  // #ifndef KALDI_PYTHON_KALDI_DECODING_ONLINE_PYTHON_GMM_DECODE_FASTER_H_
