// onl-rec/onl-rec-util.h

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
#ifndef KALDI_DEC_WRAP_UTILS_H_
#define KALDI_DEC_WRAP_UTILS_H_
#include <string>
#include "base/kaldi-common.h"
#include "fstext/fstext-lib.h"
#include "lat/kaldi-lattice.h"

#ifdef DEBUG
#include <fstream>
#endif // DEBUG

namespace kaldi {

/// \addtogroup online_latgen_utils
/// @{

// FIXME Copied from lat/lattice-functions.cc no declaration in header
// Computes (normal or Viterbi) alphas and betas; returns (total-lik, or
// best-path negated cost) 
// Note: in either case, the alphas and betas are negated costs.
// Requires that lat be topologically sorted.  This code
// will work for either CompactLattice or Latice.
template<typename LatticeType>
static double ComputeLatticeAlphasAndBetas(const LatticeType &lat,
                                           vector<double> *alpha,
                                           vector<double> *beta);

// typedef double ComputeLatticeAlphasAndBetas<CompactLattice> ComputeCompactLatticeAlphasAndBetas;


// Lattice lat has to have loglikelihoods on weights
void MovePostToArcs(const std::vector<double> &alpha,
                    const std::vector<double> &beta,
                    fst::VectorFst<fst::LogArc> * lat);


// the input lattice has to have log-likelihood weights
double CompactLatticeToWordsPost(CompactLattice &lat, fst::VectorFst<fst::LogArc> *pst);

/// @} end of "addtogroup online_latgen_utils"

} // namespace kaldi

#include "onl-rec-utils-inl.h"

#endif // KALDI_DEC_WRAP_UTILS_H_
