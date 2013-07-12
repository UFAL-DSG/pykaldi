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
#include <vector>
#include <iostream>
#include "online-python-gmm-decode-faster.h"

using namespace kaldi;

template<class FT>
FT load_function(const char *nameFce, void *lib) {
  FT f = NULL;

  dlerror();  // reset errors
  if (!lib) {
      printf("Cannot open library: %s\n", dlerror());
      return NULL;
  }   

  dlerror();  // reset errors
  f = (FT)dlsym(lib, nameFce); 
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
      printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
      dlclose(lib);
      return NULL;
  }

  return f;
}

void fill_frame(unsigned char *frame, size_t size) {
  for(size_t i=0; i<size; ++i) {
    // TODO something better
    frame[i] = 0; 
  }
}


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
  CKDW_void_t reset = load_function<CKDW_void_t>("Reset", lib);
  if (!reset) return 5;
  CKDW_frame_in_t frame_in = load_function<CKDW_frame_in_t>("FrameIn", lib);
  if (!frame_in) return 6;
  CKDW_void_t decode = load_function<CKDW_void_t>("Decode", lib);
  if (!decode) return 7;
  CKDW_prep_hyp_t prep_hyp = load_function<CKDW_prep_hyp_t>("PrepareHypothesis", lib);
  if (!prep_hyp) return 8;
  CKDW_get_hyp_t get_hyp = load_function<CKDW_get_hyp_t>("GetHypothesis", lib);
  if (!get_hyp) return 9;

  // use the loaded functions
  CKaldiDecoderWrapper d = new_Decoder(argc, argv);
  // KALDI_WARN << "DEBUG";
  
  size_t test = 40;
  for(size_t i =0; i < test; ++i) {
    size_t frame_len = 256;
    unsigned char *frame = new unsigned char[frame_len];
    fill_frame(frame, frame_len);
    // KALDI_WARN << "DEBUG";
    frame_in(d, frame, frame_len); 
    // KALDI_WARN << "DEBUG";
    decode(d);
    // KALDI_WARN << "DEBUG";
    if ( i % 2 == 0) {
      int full;
      size_t num_words = prep_hyp(d, &full);
      // KALDI_WARN << "DEBUG";
      if(full) 
        std::cout << "full hypothesis";
      else 
        std::cout << "partial hypothesis";
      std::cout << ", decoded words: " << num_words << std::endl;

      int * word_ids = new int[num_words];
      // KALDI_WARN << "DEBUG";
      get_hyp(d, word_ids, num_words);
      // KALDI_WARN << "DEBUG";
      // print the words
      for(size_t j = 0; i < num_words; ++j)
        std::cout << word_ids[j] << " ";
      std::cout << std::endl;
      delete[] word_ids;
      delete[] frame;
    }
  } // endfor
  // KALDI_WARN << "DEBUG";
  del_Decoder(d);
  // KALDI_WARN << "DEBUG";

  dlclose(lib);
  return 0;
}
