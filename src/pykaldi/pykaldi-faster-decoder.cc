// pykaldi/pykaldi-faster-decoder.cc

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

#include "pykaldi-faster-decoder.h"

namespace kaldi {

void PykaldiFasterDecoder::NewStart(void) { 
  ClearToks(toks_.Clear());

  StateId start_state = fst_.Start();
  KALDI_ASSERT(start_state != fst::kNoStateId);
  Arc dummy_arc(0, 0, Weight::One(), start_state);
  Token *dummy_token = new Token(dummy_arc, NULL);
  toks_.Insert(start_state, dummy_token);

  prev_immortal_tok_ = immortal_tok_ = dummy_token;

  frame_ = utt_frames_ = 0;
  effective_beam_ = FasterDecoder::config_.beam;
  state_ = kEndFeats;
}

} // namespace kaldi
