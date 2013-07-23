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
#include <stdlib.h>
#include <iostream>
#include "test-cffi-python-dyn.h"
#include "pykaldi-lattice-faster-decoder.h"

// function types for loading functions from shared library
typedef CWrapperLatFastDecoder* (*CWLFD_constructor_t)(void);
typedef void (*CWLFD_void_t)(CWrapperLatFastDecoder*);
typedef bool (*CWLFD_bool_t)(CWrapperLatFastDecoder*);
typedef void (*CWLFD_frame_in_t)(CWrapperLatFastDecoder*, unsigned char *, size_t);
typedef int (*CWLFD_setup_t)(CWrapperLatFastDecoder*, int, char **);
// typedef size_t (*CWLFD_size_t)(CWrapperLatFastDecoder*);
// typedef void (*CWLFD_pop_hyp_t)(CWrapperLatFastDecoder*, int *, size_t);

int main(int argc, char const *argv[]) {
  // open the library
  char nameLib[] = "libpykaldi.so";
  void *lib = dlopen(nameLib, RTLD_NOW);
  if (!lib) {
      printf("Cannot open library: %s\n", dlerror());
      return 1;
  }   
  // load functions from shared library
  CWLFD_constructor_t new_Decoder = load_function<CWLFD_constructor_t>("new_WrapperLatFastDecoder", lib);
  if (!new_Decoder) return 2;
  CWLFD_void_t del_Decoder = load_function<CWLFD_void_t>("del_WrapperLatFastDecoder", lib);
  if (!del_Decoder) return 3;
  CWLFD_setup_t setup = load_function<CWLFD_setup_t>("Setup", lib);
  if (!setup) return 4;
  CWLFD_void_t reset = load_function<CWLFD_void_t>("Reset", lib);
  if (!reset) return 5;
  CWLFD_frame_in_t frame_in = load_function<CWLFD_frame_in_t>("FrameIn", lib);
  if (!frame_in) return 6;
  CWLFD_size_t decode = load_function<CWLFD_size_t>("Decode", lib);
  if (!decode) return 7;
  CWLFD_bool_t finished = load_function<CWLFD_bool_t>("Finished", lib);
  if (!finished) return 7;
  
  // create the decoder
  CWrapperLatFastDecoder *d = new_Decoder();
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
    // FIXME at the moment frame_in does nothing
    frame_in(d, frame, frame_len);
  }
  delete[] frame;

  // decode 
  while(!finished(d)) {
    decode(d);
  }

  del_Decoder(d);
  return 0;
}
