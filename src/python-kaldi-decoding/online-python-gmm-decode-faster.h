// python-kaldi-decoding/online-wav-gmm-decode-faster.h
// -*- coding: utf-8 -*-
// FIXME rename to something like simple callback decoder

#ifndef KALDI_ONLINE_PYTHON_GMM_DECODE_FASTER_H_
#define KALDI_ONLINE_PYTHON_GMM_DECODE_FASTER_H_

#include "feat/feature-mfcc.h"
#include "online/online-audio-source.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"


#ifdef __cplusplus

/*******************
 *  C++ interface  *
 *******************/

typedef OnlineFeInput<OnlineBlockSource, Mfcc> BlockFeatInput;

// ABSOLUTALLY thread UNSAFE !
class KaldiDecoderWrapper {
 public:
  // methods designed also for calls from C
  KaldiDecoderWrapper(int argc, char **argv);  
  void set_up(int argc, char **argv);
  void reset(void);
  void frame_in(unsigned char *frame, size_t frame_len);
  void decode(kaldi::int32 char *out_str); // FIXME encode 
  virtual ~KaldiDecoderWrapper;

 private:
  int parse_args(int argc, char **argv);
  bool resetted_ = false;
  bool ready_ = false;
  bool partial_res_ = false;
  OnlineFasterDecoder::DecodeState dstate_;

  // Up to delta-delta derivative features are calculated unless LDA is used
  // Input sampling frequency is fixed to 16KHz
  const int32 kSampleFreq = 16000;
  BaseFloat acoustic_scale;
  kaldi::int32 cmn_window;
  kaldi::int32 min_cmn_window;
  kaldi::int32 right_context;
  kaldi::int32 left_context;

  kaldi::OnlineFasterDecoderOpts decoder_opts_;
  kaldi::OnlineFeatureMatrixOptions feature_reading_opts_;
  kaldi::DeltaFeaturesOptions delta_feat_opts_;
  std::string model_rxfilename_;
  std::string fst_rxfilename_;
  std::string word_syms_filename_;
  std::string silence_phones_str_;
  std::string lda_mat_rspecifier_;


  kaldi::Mfcc *mfcc_;
  OnlineBlockSource *source_;
  kaldi::BlockFeatInput *fe_input_;
  OnlineCmnInput *cmn_input_;
  kaldi::TransitionModel *trans_model_;
  fst::Fst<fst::StdArc> *decode_fst_;
  OnlineFasterDecoder *decoder_;
  VectorFst<LatticeArc> *out_fst_;
  OnlineFeatInputItf *feat_transform_;
  OnlineFeatureMatrix *feature_matrix_;
  OnlineDecodableDiagGmmScaled *decodable_;
  std::vector<kaldi::int32> silence_phones_;
  kaldi::AmDiagGmm am_gmm_;

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
void set_up(CKaldiDecoderWrapper d, int argc, char **argv);
void reset(CKaldiDecoderWrapper d);
void frame_in(CKaldiDecoderWrapper d, unsigned char *frame, size_t frame_len);
void decode(CKaldiDecoderWrapper d, char *out_str);

#ifdef __cplusplus
}
#endif



#endif  // #ifndef KALDI_ONLINE_PYTHON_GMM_DECODE_FASTER_H_
