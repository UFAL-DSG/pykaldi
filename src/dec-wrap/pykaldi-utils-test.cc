
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
  VectorFst<LogArc> *t = VectorFst<LogArc>::Read("T.fst");
  VectorFst<LogArc> *t2 = VectorFst<LogArc>::Read("T.fst");
  KALDI_ASSERT(Equal(*t, *t2, 0.001));
  delete t2;
  VectorFst<LogArc> *s = VectorFst<LogArc>::Read("S.fst");
  KALDI_ASSERT(!Equal(*t, *s, 0.001));
  delete t;
}

void test_MovePostToArc() {
  // TODO
  KALDI_ASSERT(1==2);
}

void test_LatticeToWordsPost() {
  VectorFst<LogArc> *t = VectorFst<LogArc>::Read("T.fst");
  VectorFst<LogArc> post_t;
  LatticeToWordsPost(*t, &post_t);
  delete t;
}

void test_ComputeLatticeAlphasAndBetas() {
  std::vector<std::string> tests;
  tests.push_back("T.fst");
  tests.push_back("V.fst");
  for (size_t k = 0; k < tests.size(); ++k) {
    VectorFst<LogArc> *t = VectorFst<LogArc>::Read(tests[k]);
    std::vector<double> alpha;
    std::vector<double> beta;
    double tot_post;
    tot_post = ComputeLatticeAlphasAndBetas(*t, false, &alpha, &beta);
    std::cout << "total posterior probability is " << tot_post << std::endl;
    for (size_t i = 0; i < alpha.size(); ++i) {
      std::cout << tests[k] << ": alpha[" << i << "] = " << alpha[i] 
        << " beta[" << i << "] = " << beta[i] << std::endl;
    }
  }
}

int main() {
  if(!system(NULL) || system("./testing_fst.sh"))
    std::cerr << "Maybe the testing FSTs are not created" << std::endl;
  test_fst_equal();
  test_ComputeLatticeAlphasAndBetas();
  // test_MovePostToArc();
  // test_LatticeToWordsPost();
  return 0;
}
