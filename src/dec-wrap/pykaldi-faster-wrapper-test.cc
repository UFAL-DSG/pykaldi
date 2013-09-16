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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "test-cffi-python-dyn.h"
#include "pykaldi-faster-wrapper.h"

using namespace kaldi;

// function types for loading functions from shared library
typedef CKaldiDecoderWrapper* (*CKDW_constructor_t)(void);
typedef void (*CKDW_void_t)(CKaldiDecoderWrapper*);
typedef bool (*CKDW_bool_t)(CKaldiDecoderWrapper*);
typedef size_t (*CKDW_size_t)(CKaldiDecoderWrapper*);
typedef size_t (*CKDW_fin_dec_t)(CKaldiDecoderWrapper*, bool);
typedef void (*CKDW_frame_in_t)(CKaldiDecoderWrapper*, unsigned char *, size_t);
typedef void (*CKDW_pop_hyp_t)(CKaldiDecoderWrapper*, int *, size_t);
typedef int (*CKDW_setup_t)(CKaldiDecoderWrapper*, int, char **);

int main(int argc, char **argv) {
  // open the library
  char nameLib[] = "libpykaldi.so";
  void *lib = dlopen(nameLib, RTLD_NOW);
  if (!lib) {
      printf("Cannot open library: %s\n", dlerror());
      return 1;
  }   
  // load functions from shared library
  CKDW_constructor_t new_Decoder = load_function<CKDW_constructor_t>("new_KaldiDecoderWrapper", lib);
  if (!new_Decoder) return 2;
  CKDW_void_t del_Decoder = load_function<CKDW_void_t>("del_KaldiDecoderWrapper", lib);
  if (!del_Decoder) return 3;
  CKDW_setup_t setup = load_function<CKDW_setup_t>("Setup", lib);
  if (!setup) return 4;
  CKDW_frame_in_t frame_in = load_function<CKDW_frame_in_t>("FrameIn", lib);
  if (!frame_in) return 5;
  CKDW_size_t decode = load_function<CKDW_size_t>("Decode", lib);
  if (!decode) return 6;
  CKDW_fin_dec_t finish_decoding = load_function<CKDW_fin_dec_t>("FinishDecoding", lib);
  if (!finish_decoding) return 9;
  CKDW_pop_hyp_t pop_hyp = load_function<CKDW_pop_hyp_t>("PopHyp", lib);
  if (!pop_hyp) return 10;

  // use the loaded functions
  CKaldiDecoderWrapper *d = new_Decoder();
  int retval = setup(d, argc, argv);
  if(retval != 0) {
    std::cout << "\nWrong arguments!\n"
      "Return code 0 because it is mainly used as test!\n" << std::endl;
    return 0;
  }

  unsigned char * pcm;
  std::string filename("binutils/online-data/audio/test1.wav");
  size_t pcm_size = read_16bitpcm_file(filename, &pcm);
  
  // send data in at once, use the buffering capabilities
  size_t frame_len = 2120, pcm_position = 0;
  // reading 16bit audio into char array -> 1 sample == 2 chars
  size_t frame_size = frame_len * 2; 
  unsigned char *frame = new unsigned char[frame_size];
  while(pcm_position + frame_size < pcm_size) {
    pcm_position = next_frame(frame, frame_size, pcm, pcm_position);
    frame_in(d, frame, frame_len);
  }
  delete[] frame;


  // decode() returns false if there are no more features for decoder
  size_t total_words = 0;
  for(size_t i = 0; i < 100; ++i) {
      size_t num_words = decode(d);
      int * word_ids = new int[num_words];
      pop_hyp(d, word_ids, num_words);
      printHyp(word_ids, num_words);
      delete[] word_ids;

      total_words += num_words;
  } 
  // Obtain last hypothesis
  {
      size_t num_words = finish_decoding(d, false);
      int * word_ids = new int[num_words];
      pop_hyp(d, word_ids, num_words);
      printHyp(word_ids, num_words);
      delete[] word_ids;

      total_words += num_words;
  }
  del_Decoder(d);

  std::cout << "Totally decoded words: " << total_words << std::endl;
  dlclose(lib);
  return 0;
}

