// onl-rec/onl-rec-util.cc

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov

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
#include <string>
#include "onl-rec/onl-rec-utils.h"
#include "lat/kaldi-lattice.h"
#include "fstext/fstext-utils.h"
#include "fstext/lattice-utils-inl.h"

// DEBUG
#include "lat/lattice-functions.h"

namespace kaldi {

// instantiate the template used below here
// TODO
// template double ComputeLatticeAlphasAndBetas<CompactLattice>;

void MovePostToArcs(const std::vector<double> &alpha,
                    const std::vector<double> &beta,
                    fst::VectorFst<fst::LogArc> * lat) {
  using namespace fst;
  typedef LogArc::StateId StateId;
  StateId num_states = lat->NumStates();
  for (StateId i = 0; i < num_states; ++i) {
    for (MutableArcIterator<VectorFst<LogArc> > aiter(lat, i);
        !aiter.Done();
         aiter.Next()) {
      LogArc arc = aiter.Value();
      StateId j = arc.nextstate;
      // w(i,j) = alpha(i) * w(i,j) * beta(j) / (alpha(i) * beta(i))
      // w(i,j) = w(i,j) * beta(j) / beta(i)
      double orig_w = ConvertToCost(arc.weight);
      double numer = orig_w + -beta[j];
      KALDI_VLOG(3) << "arc(" << i << ',' << j << ')' << std::endl <<
        "orig_w:" << orig_w << " beta[j=" << j << "]:" << -beta[j] <<
        " beta[i=" << i << "]:" << -beta[i] << " numer:" << numer << std::endl;
      double new_w = numer - (-beta[i]);
      KALDI_VLOG(3) << "arc orig: " << orig_w << " new: " << new_w << std::endl;
      arc.weight = LogWeight(new_w);

      aiter.SetValue(arc);
    }
  }
}

double CompactLatticeToWordsPost(CompactLattice &clat, fst::VectorFst<fst::LogArc> *pst) {

  {
    Lattice lat;
    fst::VectorFst<fst::StdArc> t_std;
    RemoveAlignmentsFromCompactLattice(&clat); // remove the alignments
    ConvertLattice(clat, &lat); // convert to non-compact form.. no new states
    ConvertLattice(lat, &t_std); // this adds up the (lm,acoustic) costs
    fst::Cast(t_std, pst);  // reinterpret the inner implementations
  }
  fst::Project(pst, fst::PROJECT_OUTPUT);


  fst::Minimize(pst);

  fst::ArcMap(pst, fst::SuperFinalMapper<fst::LogArc>());

  double tot_lik;
  std::vector<double> alpha, beta;
  fst::TopSort(pst);
  // tot_lik = ComputeCompactLatticeAlphasAndBetas(*pst, &alpha, &beta); // TODO
  tot_lik = ComputeLatticeAlphasAndBetas(*pst, &alpha, &beta);
  MovePostToArcs(alpha, beta, pst);

  return tot_lik;
}

} // namespace kaldi
