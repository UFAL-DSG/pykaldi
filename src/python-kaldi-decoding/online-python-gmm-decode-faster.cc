// onlinebin/online-gmm-decode-faster.cc

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

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
CKaldiDecoderWrapper new_KaldiDecoderWrapper() {
  return reinterpret_cast<void*>(new KaldiDecoderWrapper());
}
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper unallocate_pointer) {
  delete reinterpret_cast<KaldiDecoderWrapper*>(unallocate_pointer);
}

// methods
void set_up(CKaldiDecoderWrapper d, int argc, char **argv) {
  reinterpret_cast<KaldiDecoderWrapper*>(d)->set_up();
} 
void reset(CKaldiDecoderWrapper d) {
  reinterpret_cast<KaldiDecoderWrapper*>(d)->disconnect();
}
void frame_in(CKaldiDecoderWrapper d, unsigned char *frame, size_t frame_len) {
  reinterpret_cast<KaldiDecoderWrapper*>(d)->frame_in(frame, frame_len);
}
void decode(CKaldiDecoderWrapper d, char *out_str) {
  reinterpret_cast<KaldiDecoderWrapper*>(d)->decode();
}

/*******************
 *  C++ interface  *
 *******************/
using namespace kaldi;
using namespace fst;
typedef kaldi::int32 int32;

KaldiDecoderWrapper::KaldiDecoderWrapper(int argc, char **argv) {
  this->set_up(argc, argv);
}

~KaldiDecoderWrapper::KaldiDecoderWrapper() {
  this->reset();
}

void KaldiDecoderWrapper::reset() {

  delete this->mfcc_;
  delete this->source_;
  delete this->feat_in_;
  delete this->cmn_input_;
  delete this->trans_model_;
  delete this->decode_fst_;
  delete this->decoder_;
  delete this->out_fst_;
  delete this->feat_transform_;
  delete this->feature_matrix_;
  delete this->decodable_;
  this->silence_phones_.clear();

  this->mfcc_ = 0;
  this->source_ = 0;
  this->feat_in_ = 0;
  this->cmn_input_ = 0;
  this->trans_model_ = 0;
  this->decode_fst_ = 0;
  this->decoder_ = 0;
  this->out_fst_ = 0;
  this->feat_transform_ = 0;
  this->feature_matrix_ = 0;
  this->decodable_ = 0;

  // Up to delta-delta derivative features are calculated unless LDA is used
  // default values: order & window
  this->delta_feat_opts_ = DeltaFeaturesOptions(2, 2); 

  this->acoustic_scale = 0.1;
  this->cmn_window = 600, this->min_cmn_window = 100;
  this->right_context = 4, this->left_context = 4;

  decoder_opts_ = OnlineFasterDecoderOpts();
  feature_reading_opts_ = OnlineFeatureMatrixOptions();
  model_rxfilename_.clear();
  fst_rxfilename_.clear();
  word_syms_filename_.clear();
  silence_phones_str_.clear();
  lda_mat_rspecifier_.clear();

  this-resetted_ = true; this->ready_ = false;
}

void KaldiDecoderWrapper::set_up(int argc, char **argv) {
  this->ready_ = false; this->resetted_ = false;

  try {

    if (this->parse_args(argc, argv) != 0) {
      this->reset(); return;
    }
    {
        bool binary;
        Input ki(this->model_rxfilename_, &binary);
        this->trans_model_->Read(ki.Stream(), binary);
        this->am_gmm.Read(ki.Stream(), binary);
    }

    this->decode_fst_ = ReadDecodeGraph(this->fst_rxfilename_);
    this-> decoder_ = new OnlineFasterDecoder(*decode_fst_, this->decoder_opts_,
                                    this->silence_phones_, this->trans_model_);

    // Fixed 16 bit audio
    this->source_ = new OnlineBlockSource(); 

    // We are not properly registering/exposing MFCC and frame extraction options,
    // because there are parts of the online decoding code, where some of these
    // options are hardwired(ToDo: we should fix this at some point)
    MfccOptions mfcc_opts;
    mfcc_opts.use_energy = false;
    int32 frame_length = mfcc_opts.frame_opts.frame_length_ms = 25;
    int32 frame_shift = mfcc_opts.frame_opts.frame_shift_ms = 10;
    this->mfcc_ = new Mfcc(mfcc_opts);

    this->fe_input_ = new BlockFeatInput (this->source_, this->mfcc_,
                               frame_length * (this->kSampleFreq / 1000),
                               frame_shift * (this->kSampleFreq / 1000));
    this->cmn_input_ = new OnlineCmnInput(&fe_input, this->cmn_window, 
                                                  this->min_cmn_window);

    if (this->lda_mat_rspecifier_ != "") {
      bool binary_in;
      Matrix<BaseFloat> lda_transform; 
      Input ki(this->lda_mat_rspecifier_, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
      // lda_transform is copied to OnlineLdaInput
      this->feat_transform_ = new OnlineLdaInput(this->cmn_input_, 
                                lda_transform,
                                this->left_context, this->right_context);
    } else {
      // Note from Dan: keeping the next statement for back-compatibility,
      // but I don't think this is really the right way to set the window-size
      // in the delta computation: it should be a separate config.
      this->delta_feat_opts_.window = this->left_context / 2;
      this->feat_transform_ = new OnlineDeltaInput(this->delta_feat_opts_, 
                                                        this->cmn_input_);
    }

    // this->feature_reading_opts_ contains timeout, batch size.
    this->feature_matrix_ = new OnlineFeatureMatrix(this->feature_reading_opts_,
                                       this->feat_transform_);
    this->decodable_ = new OnlineDecodableDiagGmmScaled(this->am_gmm, 
                                            this->trans_model_, 
                                            acoustic_scale, &feature_matrix);
    this->out_fst_ = new VectorFst<LatticeArc>();

    this->partial_res_ = false;
    this->resetted_ = false; this->ready_ = true;

  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }
} // KaldiDecoderWrapper::set_up()


int KaldiDecoderWrapper::parse_args(int argc, char ** argv) {
  try {

    ParseOptions po("ParseOptions for decoder "
      "works like command line options!)\n\n"
      "Utterance segmentation is done on-the-fly.\n"
      "Feature splicing/LDA transform is used, if the optional(last) " 
      "argument is given.\n"
      "Otherwise delta/delta-delta(2-nd order) features are produced.\n\n"
      "Usage: online-gmm-decode-faster [options] <model-in>"
      "<fst-in> <word-symbol-table> <silence-phones> [<lda-matrix-in>]\n\n"
      "Example: online-gmm-decode-faster --rt-min=0.3 --rt-max=0.5 "
      "--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 "
      "model HCLG.fst words.txt '1:2:3:4:5' lda-matrix";

    // NOT USED
    // kaldi::DeltaFeaturesOptions delta_opts;
    // delta_opts.Register(&po);

    this->decoder_opts_.Register(&po, true);
    this->feature_reading_opts_.Register(&po);
    
    po.Register("left-context", &this->left_context, "Number of frames of left context");
    po.Register("right-context", &this->right_context, "Number of frames of right context");
    po.Register("acoustic-scale", &this->acoustic_scale,
                "Scaling factor for acoustic likelihoods");
    po.Register("cmn-window", &this->cmn_window,
        "Number of feat. vectors used in the running average CMN calculation");
    po.Register("min-cmn-window", &this->min_cmn_window,
                "Minumum CMN window used at start of decoding (adds "
                "latency only at start)");

    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return 1;
    }
    if (po.NumArgs() == 4)
      if (this->left_context % this->delta_feat_opts_.order != 0 || 
          this->left_context != this->right_context)
        KALDI_ERR << "Invalid left/right context parameters!";

    int32 window_size = this->right_context + thi->left_context + 1;
    this->decoder_opts_.batch_size = std::max(this->decoder_opts_.batch_size, window_size);

    this->model_rxfilename_ = po.GetArg(1),
        this->fst_rxfilename_ = po.GetArg(2),
        this->word_syms_filename_ = po.GetArg(3),
        this->silence_phones_str_ = po.GetArg(4),
        this->lda_mat_rspecifier_ = po.GetOptArg(5);

    if (!SplitStringToIntegers(this->silence_phones_str_, ":", false, &this->silence_phones_))
        KALDI_ERR << "Invalid silence-phones string " << this->silence_phones_str_;
    if (this->silence_phones_.empty())
        KALDI_ERR << "No silence phones given!";

  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }

} // KaldiDecoderWrapper::parse_args()


void frame_in(unsigned char *frame, size_t frame_len) {
  KALDI_ERR << "NOT IMPLEMENTED!";
}

void decode(kaldi::int32 char *out_str) {
  KALDI_ERR << "NOT IMPLEMENTED!";
}
