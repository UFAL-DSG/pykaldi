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

#include "util/timer.h"
#include "feat/feature-mfcc.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"
#include "online/onlinebin-util.h"
#include "online/online-audio-source.h"
#include "pykaldi-audio-source.h"
#include "pykaldi-gmm-decode-faster.h"
#include "pykaldi-feat-input.h"
#include "pykaldi-decodable.h"

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
size_t Decode(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Decode();
}
size_t DecodedWords(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->HypSize();
}
// FIXME ?Deprecated? -> use Finished and Decode instead from Python
size_t FinishDecoding(CKaldiDecoderWrapper *d) {
  // FIXME hardcoded timeout! 
  // 0 - means no timeout
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->FinishDecoding(0);
}
bool Finished(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Finished();
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
void Reset(CKaldiDecoderWrapper *d) {
  reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Reset();
}
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Setup(argc, argv);
} 

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

size_t KaldiDecoderWrapper::Decode(void) {

  decoder_->Decode(decodable_);
  // KALDI_WARN<< "HypSize before" << HypSize();

  fst::VectorFst<LatticeArc> out_fst;
  std::vector<int32> new_word_ids;
  if (UtteranceEnded()) {
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
  // KALDI_WARN<< "HypSize after " << HypSize() << " new word size " << new_word_ids.size();

  return word_ids_.size();
}

// FIXME ?Deprecated? -> use Finished and Decode instead from Python
size_t KaldiDecoderWrapper::FinishDecoding(double timeout) {
  Timer timer;
  source_->NoMoreInput();

  do {
    Decode();
    // KALDI_WARN << "DEBUG " << word_ids_.size();
    double elapsed = timer.Elapsed(); 
    if ( (timeout > 0.001) && ( elapsed > timeout)) {
      source_->DiscardAndFinish();
      // The next decode should force the decoder to output 
      // anything "buffered"
      Decode(); 
      // FIXME the beam is updated with assumption of 10ms between frames;)
      // and I am calling decode quite often here;)

      KALDI_VLOG(2) << "KaldiDecoderWrapper::FinishDecoding() timeout";
      break;
    } 
  } while(Finished()) ;

  KALDI_ASSERT(source_->BufferSize() == 0);

  // FIXME should the restart be done here? Or in separate function?
  // Last action -> prepare the decoder for new data
  // TODO it would nice to send just one "New Start" message to decoder
  // but it is a lot of work to redesign the whole decoding pipeline
  decoder_->NewStart();
  decodable_->NewStart();
  // feature_matrix_->NewStart(); it is called from decodable_->NewStart()
  source_->NewDataPromised();


  // KALDI_WARN << "DEBUG " << word_ids_.size();

  return word_ids_.size();
}

// we will use this helper function in ParseArgs
std::vector<int32> phones_to_vector(const std::string & s);

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
    opts_.Register(&po);
    
    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return 1;
    }
    if (po.NumArgs() == 4)
      if (opts_.left_context % delta_feat_opts_.order != 0 || 
          opts_.left_context != opts_.right_context)
        KALDI_ERR << "Invalid left/right context parameters!";

    int32 window_size = opts_.right_context + opts_.left_context + 1;
    decoder_opts_.batch_size = std::max(decoder_opts_.batch_size, window_size);

    opts_.model_rxfilename = po.GetArg(1);
    opts_.fst_rxfilename = po.GetArg(2);
    opts_.word_syms_filename = po.GetArg(3);

    opts_.silence_phones = phones_to_vector(po.GetArg(4));
    opts_.lda_mat_rspecifier = po.GetOptArg(5);

    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }
}  // KaldiDecoderWrapper::ParseArgs()


// Looks unficcient but: c++11 move semantics 
// and RVO: http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
// justified it. It has nicer interface.
std::vector<int32> KaldiDecoderWrapper::PopHyp(void) { 
  std::vector<int32> tmp;
  std::swap(word_ids_, tmp); // clear the word_ids_
  // KALDI_WARN << "tmp size" << tmp.size();
  return tmp; 
}

int KaldiDecoderWrapper::Setup(int argc, char **argv) {
  try {
    if (ParseArgs(argc, argv) != 0) {
      Reset(); 
      return 1;
    }

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
                               frame_length * (opts_.kSampleFreq / 1000),
                               frame_shift * (opts_.kSampleFreq / 1000));
    cmn_input_ = new OnlineCmnInput(fe_input_, opts_.cmn_window, opts_.min_cmn_window);

    if (opts_.lda_mat_rspecifier != "") {
      bool binary_in;
      Matrix<BaseFloat> lda_transform; 
      Input ki(opts_.lda_mat_rspecifier, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
      // lda_transform is copied to OnlineLdaInput
      feat_transform_ = new OnlineLdaInput(cmn_input_, 
                                lda_transform,
                                opts_.left_context, opts_.right_context);
    } else {
      // Note from Dan: keeping the next statement for back-compatibility,
      // but I don't think this is really the right way to set the window-size
      // in the delta computation: it should be a separate config.
      delta_feat_opts_.window = opts_.left_context / 2;
      feat_transform_ = new OnlineDeltaInput(delta_feat_opts_, cmn_input_);
    }

    // feature_reading_opts_ contains timeout, batch size.
    feature_matrix_ = new PykaldiFeatureMatrix(feature_reading_opts_,
                                       feat_transform_);
    decodable_ = new PykaldiDecodableDiagGmmScaled(am_gmm_, 
                                            *trans_model_, 
                                            opts_.acoustic_scale, feature_matrix_);
    return 0;
  } catch(const std::exception& e) {
    std::cerr << e.what();
    Reset();
    return 2;
  }
} // KaldiDecoderWrapper::Setup()

void KaldiDecoderWrapper::Reset(void) {

  delete mfcc_;
  delete source_;
  delete feat_transform_;
  delete cmn_input_;
  delete trans_model_;
  delete decode_fst_;
  delete decoder_;
  delete decodable_;
  word_ids_.clear();

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
  feature_reading_opts_.batch_size = 27; // 27 should be the default
  opts_ = KaldiDecoderWrapperOptions();

} // Reset ()

// converts  phones to vector representation
std::vector<int32> phones_to_vector(const std::string & s) {
  std::vector<int32> return_phones;
  if (!SplitStringToIntegers(s, ":", false, &return_phones))
      KALDI_ERR << "Invalid silence-phones string " << s;
  if (return_phones.empty())
      KALDI_ERR << "No silence phones given!";
  return return_phones;
} // phones_to_vector

} // namespace kaldi
