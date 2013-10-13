// pykaldi/pykaldi-faster-decoder.h

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov
//   Ondrej Platek

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

// FIXME the code is basically copied from online/online-faster-decoder
// 1. first reason is to get rid off dependancy on portaudio
// 2. I need slightly change the decoder and most importantly the API

#ifndef KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_
#define KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_

// FIXME is this necessary?
// #ifdef _MSC_VER
// #include <unordered_set>
// #else
// #include <tr1/unordered_set>
// #endif
// using std::tr1::unordered_set;

#include "decoder/faster-decoder.h"
#include "hmm/transition-model.h"

namespace kaldi {

// Extends the definition of FasterDecoder's options to include additional parameters. 
struct PykaldiFasterDecoderOpts : public FasterDecoderOptions {
  // Check Register function for meaning member attributes
  int32 inter_utt_sil; 
  int32 max_utt_len; 
  

  PykaldiFasterDecoderOpts() :
    inter_utt_sil(50), max_utt_len(1500) {}

  void Register(OptionsItf *po, bool full) {
    FasterDecoderOptions::Register(po, full);
    po->Register("max-utt-length", &max_utt_len,
                 "If the utterance becomes longer than this number of frames, "
                 "shorter silence is acceptable as an utterance separator");
    po->Register("inter-utt-sil", &inter_utt_sil, 
        "Maximum # of silence frames to trigger new utterance");
  }
};



/// It enables to restart the decoding for new utterance
/// and also called marked the end of utterance 
/// without the recreating the decoder.
class PykaldiFasterDecoder : public FasterDecoder {
 public:

  // "sil_phones" - the IDs of all silence phones
  PykaldiFasterDecoder(const fst::Fst<fst::StdArc> &fst,
                      const PykaldiFasterDecoderOpts &opts,
                      const std::vector<int32> &sil_phones,
                      const TransitionModel &trans_model)
      : FasterDecoder(fst, opts), opts_(opts),
        silence_set_(sil_phones), trans_model_(trans_model),
        max_beam_(opts.beam), effective_beam_(FasterDecoder::config_.beam),
        frame_(0), utt_frames_(0) 
      { ResetDecoder(true); }

  size_t Decode(DecodableInterface *decodable);

  // Makes a linear graph, by tracing back from the last "immortal" token
  // to the previous one
  bool PartialTraceback(fst::MutableFst<LatticeArc> *out_fst);

  // Makes a linear graph, by tracing back from the best currently active token
  // to the last immortal token. This method is meant to be invoked at the end
  // of an utterance in order to get the last chunk of the hypothesis
  void FinishTraceBack(fst::MutableFst<LatticeArc> *fst_out);

  // Returns "true" if the best current hypothesis ends with long enough silence
  bool EndOfUtterance();

  int32 frame() { return frame_; }

 // Change to protected for implementation of pykaldi/pykaldi-faster-decoder.cc
 protected:
  void ResetDecoder(bool full);

  // Returns a linear fst by tracing back the last N frames, beginning
  // from the best current token
  void TracebackNFrames(int32 nframes, fst::MutableFst<LatticeArc> *out_fst);

  // Makes a linear "lattice", by tracing back a path delimited by two tokens
  void MakeLattice(const Token *start,
                   const Token *end,
                   fst::MutableFst<LatticeArc> *out_fst) const;

  // Searches for the last token, ancestor of all currently active tokens
  void UpdateImmortalToken();

  const PykaldiFasterDecoderOpts opts_;
  const ConstIntegerSet<int32> silence_set_; // silence phones IDs
  const TransitionModel &trans_model_; // needed for trans-id -> phone conversion

  const BaseFloat max_beam_; // the maximum allowed beam

  BaseFloat &effective_beam_; // the currently used beam
  int32 frame_; // the next frame to be processed
  int32 utt_frames_; // # frames processed from the current utterance
  Token *immortal_tok_;      // "immortal" token means it's an ancestor of ...
  Token *prev_immortal_tok_; // ... all currently active tokens
  KALDI_DISALLOW_COPY_AND_ASSIGN(PykaldiFasterDecoder);
};

} // namespace kaldi
#endif // KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_
