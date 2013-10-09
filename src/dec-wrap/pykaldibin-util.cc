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
#include "pykaldibin-util.h"
#include "lat/kaldi-lattice.h"


void* new_fst_VectorFstLatticeArc() {
  using namespace kaldi;
  return new fst::VectorFst<LatticeArc>();
}


void print_fstMutableLatticeArc(void * fst_void) {
   fst::MutableFst<kaldi::LatticeArc> *fst = reinterpret_cast<fst::MutableFst<kaldi::LatticeArc>*>(fst_void);
    std::vector<int32> alignment;
    std::vector<int32> words;
    kaldi::LatticeWeight weight;
    fst::GetLinearSymbolSequence(*fst, &alignment, &words, &weight);

    std::cout << "words_id ";
    for (size_t i = 0; i < words.size(); i++)
      std::cout << words[i] << ' ';
    std::cout << std::endl;

    std::cout << "alignments ";
    for (size_t i = 0; i < alignment.size(); i++)
      std::cout << alignment[i] << ' ';
    std::cout << std::endl;

    std::cout << "likelihood " << -(weight.Value1() + weight.Value2()) << std::endl;
}

void del_fst_VectorFstLatticeArc(void * fst) {
   delete reinterpret_cast<fst::VectorFst<kaldi::LatticeArc>*>(fst);
}

void pykaldi_version(int *out_major, int * out_minor, int *patch) {
  *out_major = PYKALDI_MAJOR;
  *out_minor = PYKALDI_MINOR;
  *patch = PYKALDI_PATCH;
}

const char* pykaldi_git_revision() {
  std::string git_sha(PYKALDI_GIT_VERSION);
  KALDI_ASSERT((git_sha.size() == 40) && "Git SHA has length 40 size");
  return git_sha.c_str();
}


namespace kaldi {

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
