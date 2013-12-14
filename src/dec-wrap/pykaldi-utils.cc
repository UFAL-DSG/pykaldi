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


void MovePostToArcs(fst::VectorFst<fst::LogArc> * lat, 
                          const std::vector<double> &alpha,
                          const std::vector<double> &beta) {
  using namespace fst;
  typedef typename LogArc::StateId StateId;
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
#ifdef DEBUG
  std::string lattice_wspecifier("ark:|gzip - c > after_getLattice.gz");
  CompactLatticeWriter compact_lattice_writer;
  compact_lattice_writer.Open(lattice_wspecifier);
  compact_lattice_writer.Write("unknown", clat);
  compact_lattice_writer.Close();
#endif // DEBUG

  {
    Lattice lat;
    fst::VectorFst<fst::StdArc> t_std;
    RemoveAlignmentsFromCompactLattice(&clat); // remove the alignments
    ConvertLattice(clat, &lat); // convert to non-compact form... no new states
#ifdef DEBUG
    LatticeWriter lattice_writer;
    std::string lattice_wspecifier("ark:|gzip - c > after_convertLattice_lat.gz");
    compact_lattice_writer.Open(lattice_wspecifier);
    compact_lattice_writer.Write("unknown", clat);
    compact_lattice_writer.Close();
#endif // DEBUG
    ConvertLattice(lat, &t_std); // this adds up the (lm,acoustic) costs to tropical fst
#ifdef DEBUG
    std::ofstream logfile;
    logfile.open("after_convert_trop.fst");
    t_std.Write(logfile, fst::FstWriteOptions());
    logfile.close();
#endif // DEBUG
    fst::Cast(t_std, pst);  // reinterpret the inner implementations
  }
#ifdef DEBUG
  {
    std::ofstream logfile;
    logfile.open("after_cast_c.fst");
    pst->Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG
  fst::Project(pst, fst::PROJECT_OUTPUT);


//   fst::Minimize(pst);
// #ifdef DEBUG
//   {
//     std::ofstream logfile;
//     logfile.open("after_minimize_c.fst");
//     pst->Write(logfile, fst::FstWriteOptions());
//     logfile.close();
//   }
// #endif // DEBUG

  std::vector<double> alpha, beta;
  double tot_prob;
//   fst::TopSort(pst);
//   tot_prob = ComputeLatticeAlphasAndBetas(*pst, &alpha, &beta);
//   MovePostToArcs(pst, alpha, beta);
// #ifdef DEBUG
//   for (size_t i = 0; i < alpha.size(); ++i) {
//     std::cerr << "a[" << i << "] = " << alpha[i] << " beta[" << i << "] = "
//       << beta[i] << std::endl;
//   }
//   {
//     std::ofstream logfile;
//     logfile.open("after_post_c.fst");
//     pst->Write(logfile, fst::FstWriteOptions());
//     logfile.close();
//   }
// #endif // DEBUG

  return tot_prob;
}



double LatticeToWordsPost(Lattice &lat, fst::VectorFst<fst::LogArc> *pst) {
  // the input lattice has to have log-likelihood weights
  fst::VectorFst<fst::LogArc> t;  // tmp object
  {
    fst::VectorFst<fst::StdArc> t_std;
    ConvertLattice(lat, &t_std); // Does it convert to tropical? If yes that is not nice
    fst::Cast(t_std, &t);  // reinterpret the inner implementations
  }
#ifdef DEBUG
  {
    std::ofstream logfile;
    logfile.open("after_Cast.fst");
    t.Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG
  fst::Project(&t, fst::PROJECT_OUTPUT);

  // TODO http://kaldi.sourceforge.net/fst_algo.html
  // fst::RemoveEpsLocal(&t); // kaldi alternative TODO extremaly slow
  fst::RmEpsilon(&t); // original fst
#ifdef DEBUG
  {
    std::ofstream logfile;
    logfile.open("after_RmEpsilon.fst");
    t.Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG

  fst::ILabelCompare<fst::LogArc> ilabel_comp;
  fst::ArcSort(&t, ilabel_comp);
  fst::Determinize(t, pst);
  fst::Connect(pst);
#ifdef DEBUG
  {
    std::ofstream logfile;
    logfile.open("after_Determinize.fst");
    pst->Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG

  std::vector<double> alpha, beta;
  double tot_prob;
  fst::TopSort(pst);
  tot_prob = ComputeLatticeAlphasAndBetas(*pst, &alpha, &beta);
  MovePostToArcs(pst, alpha, beta);
#ifdef DEBUG
  for (size_t i = 0; i < alpha.size(); ++i) {
    std::cerr << "a[" << i << "] = " << alpha[i] << " beta[" << i << "] = "
      << beta[i] << std::endl;
  }
  {
    std::ofstream logfile;
    logfile.open("after_post.fst");
    pst->Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG

fst::Minimize(pst);
#ifdef DEBUG
  {
    std::ofstream logfile;
    logfile.open("after_minimize.fst");
    pst->Write(logfile, fst::FstWriteOptions());
    logfile.close();
  }
#endif // DEBUG

  return tot_prob;
}

double LatticeToNbest(const fst::VectorFst<fst::StdArc> &lat, 
                     std::vector<std::vector<int> > &nbest, 
                     std::vector<BaseFloat> &prob_out,
                     int n) {
  // TODO compute likelihoods and normalize them against each other
  // There are n - eps arcs from 0 state which mark beginning of each list
  // Following one path there are 2 eps arcs at beginning
  // and one at the end before final state

  // using namespace fst;
  // VectorFst<StdArc> p;
  // ShortestPath(lat, &p, n); 
  return 1.0;
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
