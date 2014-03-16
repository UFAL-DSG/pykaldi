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
#include "onl-rec/onl-rec-latgen-recogniser.h"
#include "onl-rec/onl-rec-utils.h"
#include "fstext/fstext-utils.h"

using namespace kaldi;
using std::string;


size_t ReadWav(const std::string & wav_path, unsigned char **arr_out) {

  std::ifstream is(wav_path.c_str());
  WaveData wave;
  wave.Read(is);
  Matrix<BaseFloat> data(wave.Data());
  KALDI_ASSERT(data.NumRows() == 1);

  delete[] (*arr_out);
  (*arr_out) = new unsigned char[data.NumCols()];
  for (size_t i = 0; i < data.NumCols(); ++i) {
    (*arr_out)[i] = data(0, i);
  }

  std::cout << std::endl << " Waveform loaded" << std::endl << std::endl;

  return data.NumCols();
}


bool recognitionOn(size_t simAudioProcessed, size_t simAudioSize) {
  return simAudioProcessed < (simAudioSize - 1);
}


size_t getAudio(unsigned char* simAudioInput, size_t simAudioSize, 
    size_t *simAudioProcessed, unsigned char **audio_array) {

  delete [] (*audio_array);
  std::cout << std::endl << " Previous chunk deleted." << std::endl << std::endl;
  size_t chunk_size = 4560; // TODO comment why?
  chunk_size = std::min(chunk_size, ((simAudioSize - 1) - (*simAudioProcessed)));
  (*audio_array) = new unsigned char[chunk_size];
  for (size_t i = 0; i < chunk_size; ++i) {
    (*audio_array)[i] = simAudioInput[*simAudioProcessed]; 
    (*simAudioProcessed)++;
  }
  return chunk_size;
}


int main(int argc, char *argv[]) {
  std::vector<string> args(argv, argv+argc);
  if (args.size() < 2) {
    std::cerr << "Usage: " << args[0] << ": <wave_file> <GmmLatgenOptions>" 
      << std::endl;
    return 0;
  }

  size_t simAudioProcessed = 0;
  unsigned char* simAudioInput = NULL;
  // Asuming default 16Khz sampling, 16Bit=2B. Can be changed in  audio buffer options.
  size_t simAudioSize = ReadWav(args[1], &simAudioInput);

  OnlineLatgenRecogniser rec;
  // Pass commandline arguments except for the wave file name.
  rec.Setup(args.size() - 2, argv + 2);

  std::cout << std::endl << " Recogniser initialized." << std::endl << std::endl;

  unsigned char * audio_array = NULL;
  size_t decoded_frames = 0; 
  size_t decoded_now = 0; 
  size_t max_decoded = 10;
  while(recognitionOn(simAudioProcessed, simAudioSize)) {
    std::cout << std::endl << " Still some new audio for forward decoding." << std::endl << std::endl;
    size_t audio_len = getAudio(simAudioInput, simAudioSize, &simAudioProcessed, &audio_array);
    std::cout << std::endl << " Audio chunk prepared. Size " << audio_len << std::endl << std::endl;
    rec.FrameIn(audio_array, audio_len);
    std::cout << std::endl << " Audio chunk buffered into decoder." << std::endl << std::endl;
    do {
      decoded_frames += decoded_now;
      decoded_now = rec.Decode(max_decoded);
    } while(decoded_now > 0);
    std::cout << std::endl << " Audio chunk forward-decoded." << std::endl << std::endl;
  }

  rec.PruneFinal();
  
  double tot_lik = 0; 
  fst::VectorFst<fst::LogArc> word_post_lat;
  rec.GetLattice(&word_post_lat, &tot_lik);

  std::cout << std::endl << " Posterior lattice extracted." << std::endl << std::endl;
  std::cout << "Likelihood of the utterance is " << tot_lik << std::endl;

  // Writing the word posterior lattice to file
  std::ofstream logfile;
  string fstname = args[1] + ".fst";
  logfile.open(fstname.c_str());
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

  bool clear_data = false;  // True: The buffered unprocessed audio will be cleared
  rec.Reset(clear_data);  // Ready for new utterance

  std::cout << std::endl << " Recogniser resetted and prepared for next utterance." << std::endl << std::endl;

  return 0;
}
