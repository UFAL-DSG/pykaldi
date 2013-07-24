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

#ifndef KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_
#define KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_

#include "online/online-faster-decoder.h"

namespace kaldi {

/// Simple extension of OnlineFasterDecoder
/// It enables to restart the decoding for new utterance
/// and also called marked the end of utterance 
/// without the recreating the decoder.
class PykaldiFasterDecoder : public OnlineFasterDecoder {
 public:

  // "sil_phones" - the IDs of all silence phones
  PykaldiFasterDecoder(const fst::Fst<fst::StdArc> &fst,
                      const OnlineFasterDecoderOpts &opts,
                      const std::vector<int32> &sil_phones,
                      const TransitionModel &trans_model)
      : OnlineFasterDecoder(fst, opts, sil_phones , trans_model) {}
  void NewStart(void);

  KALDI_DISALLOW_COPY_AND_ASSIGN(PykaldiFasterDecoder);
};

} // namespace kaldi
#endif // KALDI_PYKALDI_PYKALDI_FASTER_DECODER_H_
