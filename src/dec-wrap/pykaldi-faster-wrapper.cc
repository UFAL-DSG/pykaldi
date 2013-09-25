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
#include "pykaldibin-util.h"
#include "pykaldi-audio-source.h"
#include "pykaldi-faster-wrapper.h"
#include "pykaldi-feat-input.h"
#include "pykaldi-decodable.h"

/*****************
 *  C interface  *
 *****************/
// explicit constructor and destructor
CKaldiDecoderWrapper *new_KaldiDecoderWrapper(void) {
  return reinterpret_cast<CKaldiDecoderWrapper*>(new kaldi::KaldiDecoderWrapper());
}
void del_KaldiDecoderWrapper(CKaldiDecoderWrapper* d) {
  delete reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
}

// methods from C
size_t Decode(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Decode();
}
size_t HypSize(CKaldiDecoderWrapper *d) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->HypSize();
}
size_t FinishDecoding(CKaldiDecoderWrapper *d, int clear_input) {
  kaldi::KaldiDecoderWrapper *dp = reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d);
  return dp->FinishDecoding(clear_input);
}
int Finished(CKaldiDecoderWrapper *d) {
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
int Setup(CKaldiDecoderWrapper *d, int argc, char **argv) {
  return reinterpret_cast<kaldi::KaldiDecoderWrapper*>(d)->Setup(argc, argv);
} 

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

// we will use this helper function in ParseArgs
std::vector<int32> phones_to_vector(const std::string & s);

size_t KaldiDecoderWrapper::Decode(void) {
  KALDI_VLOG(2) << "Finished: " << Finished()
                << " input size: " << source_->BufferSize();

  decoder_->Decode(decodable_);

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

size_t KaldiDecoderWrapper::FinishDecoding(bool clear_input) {
  KALDI_VLOG(1) << "before while: input size: " << source_->BufferSize()
                << " decoder_->Finished() " << Finished();
  while(!Finished()) {
    Decode();
  }
  KALDI_VLOG(1) << "after while: input size: " << source_->BufferSize()
                << " decoder_->Finished() " << Finished();

  // FIXME should the restart be done here? Or in separate function?
  // Last action -> prepare the decoder for new data
  // TODO it would nice to send just one "New Start" message to decoder
  // but it is a lot of work to redesign the whole decoding pipeline
  KALDI_VLOG(2) << "word_ids_.size() " << word_ids_.size();
  return word_ids_.size();
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
  KALDI_VLOG(2) << "tmp.size() " << tmp.size();
  return tmp; 
}

int KaldiDecoderWrapper::Setup(int argc, char **argv) {
  try {
    if (ParseArgs(argc, argv) != 0) {
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
    source_ = new PykaldiBuffSource(); 

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
    return 2;
  }
} // KaldiDecoderWrapper::Setup()

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
