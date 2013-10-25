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

void lattice2row(const Lattice&lat, 
                std::vector<int> &out_ids, BaseFloat *prob) {
  CompactLattice clat;
  ConvertLattice(lat, &clat); // write in compact form.
  // TODO extract the data from clat
  // DEBUG
  out_ids.push_back(99);
  *prob = 1.0;
}

void lattice2nbest(const Lattice &lat, int n, 
        std::vector<std::vector<int> > &out_nbest, 
        std::vector<BaseFloat> & out_prob) {
  KALDI_WARN << "DEBUG";

  std::vector<Lattice> nbest_lats;
  fst::NbestAsFsts(lat, n, &nbest_lats);
  for (int32 k = 0; k < static_cast<int32>(nbest_lats.size()); ++k) {
    std::vector<int> row;
    BaseFloat prob;
    lattice2row(nbest_lats[k], row, &prob);
    out_nbest.push_back(row);  // copying the vector
    out_prob.push_back(prob);

    // TODO DEBUGGING replace wrinting 
      // CompactLattice nbest_clat;
      // ConvertLattice(nbest_lats[k], &nbest_clat); // write in compact form.
      // std::ofstream f;
      // f.open("nbest.lat", std::ios::binary);
      // fst::FstWriteOptions opts;  // in fst/fst.h
      // nbest_clat.Write(f, opts);
      // f.close();
  }
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
