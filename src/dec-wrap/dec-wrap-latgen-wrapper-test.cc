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

#include "base/kaldi-types.h"
#include "base/kaldi-math.h"
#include "feat/wave-reader.h"
#include "dec-wrap/dec-wrap-latgen-wrapper.h"
#include "dec-wrap/dec-wrap-utils.h"
#include "fstext/fstext-utils.h"

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


bool recognitionOn(size_t simAudioProcessed, size_t simAudioSize) {
  return simAudioProcessed < (simAudioSize - 1);
}


size_t getAudio(unsigned char* simAudioInput, size_t simAudioSize, size_t simAudioProcessed, unsigned char *new_audio_array) {
  delete [] new_audio_array;
  size_t chunk_size = 4560; // TODO comment why?
  chunk_size = std::min(chunk_size, ((simAudioSize - 1) - simAudioProcessed));
  new_audio_array = new unsigned char[chunk_size];
  for (size_t i = 0; i < chunk_size; ++i) {
    simAudioProcessed++;
    new_audio_array[i] = simAudioInput[simAudioProcessed]; 
  }
  return chunk_size;
}


size_t ConvertVector2RawData(Vector<BaseFloat> &v_in, unsigned char *arr_out) {
  return v_in.Dim();
}


int main(int argc, char *argv[]) {
  std::vector<string> args(argv, argv+argc);
  if (args.size() < 2) {
    std::cerr << "Usage: " << args[0] << ": <wave_file> <GmmLatgenOptions>" 
      << std::endl;
    return 0;
  }

  Vector<BaseFloat> wave;
  ReadWav(args[1], wave);

  size_t simAudioProcessed = 0;
  unsigned char* simAudioInput = NULL;
  // Asuming default 16Khz sampling, 16Bit=2B. Can be changed in  audio buffer options.
  size_t simAudioSize = ConvertVector2RawData(wave, simAudioInput);

  GmmLatgenWrapper rec;
  // Pass commandline arguments except for the wave file name.
  rec.Setup(args.size() - 2, argv + 2);

  unsigned char * audio_array = NULL;
  size_t decoded_frames = 0; 
  size_t decoded_now = 0; 
  size_t max_decoded = 10;
  while(recognitionOn(simAudioProcessed, simAudioSize)) {
    size_t audio_len = getAudio(simAudioInput, simAudioSize, simAudioProcessed, audio_array);
    rec.FrameIn(audio_array, audio_len);
    do {
      decoded_frames += decoded_now;
      decoded_now = rec.Decode(max_decoded);
    } while(decoded_now > 0);
  }

  rec.PruneFinal();
  
  double tot_lik = 0; 
  fst::VectorFst<fst::LogArc> word_post_lat;
  rec.GetLattice(&word_post_lat, &tot_lik);

  bool clear_data = false;  // True: The buffered unprocessed audio will be cleared
  rec.Reset(clear_data);  // Ready for new utterance

  std::cout << "Likelihood of the utterance is " << tot_lik << std::endl;

  // Writing the word posterior lattice to file
  std::ofstream logfile;
  logfile.open("latgen-wrapper-test.fst");
  word_post_lat.Write(logfile, fst::FstWriteOptions());
  logfile.close();

  // // Optional printing the nbest list from the lattice
  // std::vector<std::vector<int> > word_ids_nbest;
  // LatticeToVectorNbest(word_post_lat, &word_ids_nbest); // TODO not yet implemented
  // for(size_t n ; n < word_ids_nbest.size(); ++n) {
  //   for(size_t i; i < word_ids_nbest[n].size(); ++i) {
  //     std::cout<< i << ' ';
  //   }
  //   std::cout << std::endl;
  // }

  return 0;
}
