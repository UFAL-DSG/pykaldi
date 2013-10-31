// pykaldi/pykaldi-util.cc

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
#include "dec-wrap/pykaldi-utils.h"
#include "lat/kaldi-lattice.h"
#include "fstext/fstext-utils.h"
#include "fstext/lattice-utils-inl.h"


namespace kaldi {

void pykaldi_version(int *out_major, int * out_minor, int *patch) {
  *out_major = PYKALDI_MAJOR;
  *out_minor = PYKALDI_MINOR;
  *patch = PYKALDI_PATCH;
}

void build_git_revision(std::string & pykaldi_git_revision) {
  pykaldi_git_revision.clear();
  pykaldi_git_revision.append(PYKALDI_GIT_VERSION);
  KALDI_ASSERT((pykaldi_git_revision.size() == 40) && "Git SHA has length 40 size");
}



// FIXME Copied from lat/lattice-functions.cc 
// There is no no declaration in lat/lattice-functions.h!
// Computes (normal or Viterbi) alphas and betas; returns (total-prob, or
// best-path negated cost) Note: in either case, the alphas and betas are
// negated costs.  Requires that lat be topologically sorted.  This code
// will work for either CompactLattice or Latice.
template<typename LatticeType>
static double ComputeLatticeAlphasAndBetas(const LatticeType &lat,
                                           bool viterbi,
                                           vector<double> *alpha,
                                           vector<double> *beta) {
  typedef typename LatticeType::Arc Arc;
  typedef typename Arc::Weight Weight;
  typedef typename Arc::StateId StateId;

  StateId num_states = lat.NumStates();
  KALDI_ASSERT(lat.Properties(fst::kTopSorted, true) == fst::kTopSorted);
  KALDI_ASSERT(lat.Start() == 0);
  alpha->resize(num_states, kLogZeroDouble);
  beta->resize(num_states, kLogZeroDouble);

  double tot_forward_prob = kLogZeroDouble;
  (*alpha)[0] = 0.0;
  // Propagate alphas forward.
  for (StateId s = 0; s < num_states; s++) {
    double this_alpha = (*alpha)[s];
    for (fst::ArcIterator<LatticeType> aiter(lat, s); !aiter.Done();
         aiter.Next()) {
      const Arc &arc = aiter.Value();
      double arc_like = -ConvertToCost(arc.weight);
      (*alpha)[arc.nextstate] = LogAddOrMax(viterbi, (*alpha)[arc.nextstate],
                                                this_alpha + arc_like);
    }
    Weight f = lat.Final(s);
    if (f != Weight::Zero()) {
      double final_like = this_alpha - ConvertToCost(f);
      tot_forward_prob = LogAddOrMax(viterbi, tot_forward_prob, final_like);
    }
  }
  for (StateId s = num_states-1; s >= 0; s--) { // it's guaranteed signed.
    double this_beta = -ConvertToCost(lat.Final(s));
    for (fst::ArcIterator<LatticeType> aiter(lat, s); !aiter.Done();
         aiter.Next()) {
      const Arc &arc = aiter.Value();
      double arc_like = -ConvertToCost(arc.weight),
          arc_beta = (*beta)[arc.nextstate] + arc_like;
      this_beta = LogAddOrMax(viterbi, this_beta, arc_beta);
    }
    (*beta)[s] = this_beta;
  }
  double tot_backward_prob = (*beta)[lat.Start()];
  if (!ApproxEqual(tot_forward_prob, tot_backward_prob, 1e-8)) {
    KALDI_WARN << "Total forward probability over lattice = " << tot_forward_prob
               << ", while total backward probability = " << tot_backward_prob;
  }
  // Split the difference when returning... they should be the same.
  return 0.5 * (tot_backward_prob + tot_forward_prob);
}


fst::Fst<fst::StdArc> *ReadDecodeGraph(std::string filename) {
  // read decoding network FST
  Input ki(filename); // use ki.Stream() instead of is.
  if (!ki.Stream().good()) KALDI_ERR << "Could not open decoding-graph FST "
                                      << filename;

  fst::FstHeader hdr;
  if (!hdr.Read(ki.Stream(), "<unknown>")) {
    KALDI_ERR << "Reading FST: error reading FST header.";
  }
  if (hdr.ArcType() != fst::StdArc::Type()) {
    KALDI_ERR << "FST with arc type " << hdr.ArcType() << " not supported.\n";
  }
  fst::FstReadOptions ropts("<unspecified>", &hdr);

  fst::Fst<fst::StdArc> *decode_fst = NULL;

  if (hdr.FstType() == "vector") {
    decode_fst = fst::VectorFst<fst::StdArc>::Read(ki.Stream(), ropts);
  } else if (hdr.FstType() == "const") {
    decode_fst = fst::ConstFst<fst::StdArc>::Read(ki.Stream(), ropts);
  } else {
    KALDI_ERR << "Reading FST: unsupported FST type: " << hdr.FstType();
  }
  if (decode_fst == NULL) { // fst code will warn.
    KALDI_ERR << "Error reading FST (after reading header).";
    return NULL;
  } else {
    return decode_fst;
  }
}


void PrintPartialResult(const std::vector<int32>& words,
                        const fst::SymbolTable *word_syms,
                        bool line_break) {
  KALDI_ASSERT(word_syms != NULL);
  for (size_t i = 0; i < words.size(); i++) {
    std::string word = word_syms->Find(words[i]);
    if (word == "")
      KALDI_ERR << "Word-id " << words[i] <<" not in symbol table.";
    std::cout << word << ' ';
  }
  if (line_break)
    std::cout << "\n\n";
  else
    std::cout.flush();
}


// converts  phones to vector representation
std::vector<int32> phones_to_vector(const std::string & s) {
  std::vector<int32> return_phones;
  if (!SplitStringToIntegers(s, ":", false, &return_phones))
      KALDI_ERR << "Invalid silence-phones string " << s;
  if (return_phones.empty())
      KALDI_ERR << "No silence phones given!";
  return return_phones;
} // phones_to_vector

} // namespace kaldi
