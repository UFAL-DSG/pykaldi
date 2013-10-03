// -*- coding: utf-8 -*-
/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
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
#ifndef KALDI_PYKALDI_LATGEN_DECODER_H_
#define KALDI_PYKALDI_LATGEN_DECODER_H_

#include "decoder/lattice-faster-decoder.h"
#include "fst/fstlib.h"

namespace kaldi {

class PykaldiLatticeFasterDecoder: public LatticeFasterDecoder {
 public:
  PykaldiLatticeFasterDecoder(const fst::Fst<fst::StdArc> &fst,
                       const LatticeFasterDecoderConfig &config)
    : LatticeFasterDecoder(fst, config) {}

};

}  // namespace kaldi


#endif  // #ifndef KALDI_PYKALDI_LATGEN_DECODER_H_
