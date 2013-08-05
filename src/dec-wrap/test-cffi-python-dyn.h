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


#ifndef KALDI_PYTHON_KALDI_DECODING_TEST_CFFI_PYTHON_DYN_H_
#define KALDI_PYTHON_KALDI_DECODING_TEST_CFFI_PYTHON_DYN_H_
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

template<class FT> FT load_function(const char *nameFce, void *lib) {
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

void printHyp(int *word_ids, size_t num_words) {
  std::cout << ", decoded words: " << num_words << std::endl;
  // print the words
  for(size_t j = 0; j < num_words; ++j)
    std::cout << word_ids[j] << " ";
  std::cout << std::endl;
}


void fill_frame_random(unsigned char *frame, size_t size) {
  for(size_t i=0; i<size; ++i) {
    frame[i] = random() & 0xff;
  }
}

/// Read the file like it is 16bit raw pcm
size_t read_16bitpcm_file(const std::string & filename, unsigned char **pcm) {
  size_t size = 0;
  *pcm = NULL;

  std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
  if (file.is_open()) {
    size = file.tellg();
    *pcm = new unsigned char [size];
    file.seekg(0, std::ios::beg);
    file.read((char *)*pcm, size);
    file.close();
  }
  return size;
}

size_t next_frame(unsigned char * data_target, size_t target_size,
                  unsigned char *src, size_t src_position) {
  for(size_t i=0; i < target_size; ++i) {
    data_target[i] = src[src_position + i];
  }
  return src_position + target_size;
}


/*******************************************************
 *  Tiny Wrapper (like compute-wer) testing functions  *
 *******************************************************/

typedef int (*f_t)(int c, char **ar);

int testSharedLib(int argc, char ** argv, const char *nameFce, const char *nameLib) {
    void *lib = dlopen(nameLib, RTLD_NOW);
    if (!lib) {
        printf("Cannot open library: %s\n", dlerror());
        return 1;
    }   

    dlerror();  // reset errors
    f_t f = (f_t)dlsym(lib, nameFce); 
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    // using the function
    int retval = f(argc, argv);
    
    dlclose(lib);
    return retval;
}

int testSharedLibS(int argc, char ** argv, const char *nameFce) {
    return testSharedLib(argc, argv, nameFce, "libpykaldi.so");
}
#endif // #ifndef KALDI_PYTHON_KALDI_DECODING_TEST_CFFI_PYTHON_DYN_H_
