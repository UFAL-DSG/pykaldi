// feat/feature-fbank-test.cc

// Copyright 2014 Ondrej Platek 

// See ../../COPYING for clarification regarding multiple authors
//
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

#include <iostream>
#include <string>
#include <vector>

#include "base/kaldi-math.h"
#include "feat/wave-reader.h"
#include "dec-wrap/dec-wrap-latgen-wrapper.h"

using namespace kaldi;
using std::string;

void ReadWav(const std::string & wav_path, Vector<BaseFloat> &out_v) {

  std::cout << "<<<=== Reading waveform\n";

  std::ifstream is(wav_path.c_str());
  WaveData wave;
  wave.Read(is);
  Matrix<BaseFloat> data(wave.Data());
  KALDI_ASSERT(data.NumRows() == 1);
  out_v.Resize(data.NumCols());
  out_v.CopyFromVec(data.Row(0));
}

int main(int argc, char *argv[])
{
  std::vector<string> args(argv, argv+argc);
  if (args.size() < 2) {
    std::cerr << "Usage: " << args[0] << ": <wave_file> <GmmLatgenOptions>" 
      << std::endl;
  }
  Vector<BaseFloat> wave;
  ReadWav(args[1], wave);

  GmmLatgenWrapper rec;
  rec.Setup(args.size() - 2, argv + 2);

  // TOOD the use case is to switch FrameIn and decode
  // often so the decoder can decode as the user speaks
  rec.FrameIn(wave_to_std_vector, len_wave);

  size_t decoded_frames = 0;
  size_t decoded_now = 0;
  size_t max_decoded = 10;
  do {
    decoded_frames += decoded_now;
    decode_now = rec.Decode(max_decoded);
  } while(decoded_now > 0);

  rec.PruneFinal();
  
  fst::VectorFst<fst::LogArc> word_post_lat;
  double tot_lik;
  rec.GetLattice(&word_post_lat, &tot_lik);

  std::cout << "Likelihood of the utterance is " << tot_lik << std::endl;

  // TODO convert lattice to tropical semiring
  // TODO SingleShortestPaht http://openfst.cs.nyu.edu/doxygen/fst/html/shortest-path_8h.html
  // TODO write shortest path labels to output

  return 0;
}
