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
#include <dlfcn.h>
#include <stdio.h>



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
