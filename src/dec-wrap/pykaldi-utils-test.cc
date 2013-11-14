
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
#include "dec-wrap/pykaldi-utils.h"
#include "dec-wrap/fst-equal.h"
#include <fst/fst.h>
#include <stdlib.h>

using namespace kaldi;
using namespace fst;

void test_fst_equal() {
  VectorFst<StdArc> *t = VectorFst<StdArc>::Read("T.fst");
  VectorFst<StdArc> *t2 = VectorFst<StdArc>::Read("T.fst");
  KALDI_ASSERT(Equal(*t, *t2, 0.001));
  delete t2;
  VectorFst<StdArc> *s = VectorFst<StdArc>::Read("S.fst");
  KALDI_ASSERT(!Equal(*t, *s, 0.001));
  delete t;
}

void test_MovePostToArc() {
  // TODO
  KALDI_ASSERT(1==2);
}

void test_LatticeToWordsPost() {
  // TODO
  KALDI_ASSERT(1==2);
}

int main() {
  if(!system(NULL) || system("./testing_fst.sh"))
    std::cerr << "Maybe the testing FSTs are not created" << std::endl;
  test_fst_equal();
  // test_MovePostToArc();
  // test_LatticeToWordsPost();
  return 0;
}
