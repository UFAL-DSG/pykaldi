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
#include "pykaldi-audio-source.h"
#include "matrix/kaldi-vector.h"

using namespace kaldi;

void test_ReadEmpty(const PykaldiBuffSourceOptions & opts) {
  int32 dim = 8;
  Vector<BaseFloat> to_fill;
  to_fill.Resize(dim);
  PykaldiBuffSource s(opts);

  int32 read = s.Read(&to_fill);
  KALDI_ASSERT(read == 0);
  KALDI_ASSERT(to_fill.Dim() == dim);
}


int main() {
  PykaldiBuffSourceOptions opts;
  test_ReadEmpty(opts);
  return 0;
}

