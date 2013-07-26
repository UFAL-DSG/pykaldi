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

#include "util/timer.h"
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

PykaldiFasterDecoder::DecodeState
PykaldiFasterDecoder::Decode(DecodableInterface *decodable) {
  if (state_ == kEndFeats || state_ == kEndUtt) // new utterance
    ResetDecoder(state_ == kEndFeats);
  ProcessNonemitting(std::numeric_limits<float>::max());
  int32 batch_frame = 0;
  Timer timer;
  double64 tstart = timer.Elapsed(), tstart_batch = tstart;
  // BaseFloat factor = -1;
  for (; !decodable->IsLastFrame(frame_) && batch_frame < opts_.batch_size;
       ++frame_, ++utt_frames_, ++batch_frame) {
    //   // FIXME solve the update beam and remove the hardcoded dependancy!
    //   // warning: hardcoded 10ms frames assumption!
    // if (batch_frame != 0 && (batch_frame % opts_.update_interval) == 0) {
    //   // adjust the beam if needed
    //   BaseFloat tend = timer.Elapsed();
    //   BaseFloat elapsed = (tend - tstart) * 1000;
    //   factor = elapsed / (opts_.rt_max * opts_.update_interval * 10);
    //   BaseFloat min_factor = (opts_.rt_min / opts_.rt_max);
    //   if (factor > 1 || factor < min_factor) {
    //     BaseFloat update_factor = (factor > 1)?
    //         -std::min(opts_.beam_update * factor, opts_.max_beam_update):
    //          std::min(opts_.beam_update / factor, opts_.max_beam_update);
    //     effective_beam_ += effective_beam_ * update_factor;
    //     effective_beam_ = std::min(effective_beam_, max_beam_);
    //   }
    //   tstart = tend;
    // }
    if (batch_frame != 0 && (frame_ % 200) == 0)
      // one log message at every 2 seconds assuming 10ms frames
      KALDI_VLOG(5) << "Beam: " << effective_beam_
          << "; Speed: "
          << ((timer.Elapsed() - tstart_batch) * 1000) / (batch_frame*10)
          << " xRT";
    BaseFloat weight_cutoff = ProcessEmitting(decodable, frame_);
    ProcessNonemitting(weight_cutoff);
  }
  if (batch_frame == opts_.batch_size && !decodable->IsLastFrame(frame_)) {
    if (EndOfUtterance())
      state_ = kEndUtt;
    else
      state_ = kEndBatch;
  } else {
    state_ = kEndFeats;
  }
  return state_;
}

} // namespace kaldi
