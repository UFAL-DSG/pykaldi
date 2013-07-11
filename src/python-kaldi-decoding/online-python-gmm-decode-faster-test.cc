#include "online-python-gmm-decode-faster.h"

using namespace kaldi;

int main(int argc, char **argv) {
    // FIXME do not use the decoder via command line argumets
    // FIXME implement real test which should simulate the calls from Python
    // without arguments it returns return code 1 -> we want 0 

    void *lib = dlopen("libpykaldi.so", RTLD_NOW);
    if (!lib) {
        printf("Cannot open library: %s\n", dlerror());
        return 1;
    }   
    char name_get_fce[] = "get_online_python_gmm_decode_faster";

    dlerror();  // reset errors
    get_decoder_t get_online_python_gmm_decode_faster = (get_decoder_t)dlsym(lib, name_get_fce); 
    char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", name_get_fce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    dlerror();  // reset errors
    char name_decode_fce[] = "decode";
    decode_t decode = (decode_t)dlsym(lib, name_decode_fce); 
    dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", name_decode_fce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    // Using the functions
    int retval;

    // get the decoder and other objects needed 
    OnlineFasterDecoder *decoder = NULL; 
    OnlineDecodableDiagGmmScaled *decodable = NULL;
    OnlineFeatInputItf *feat_transform = NULL;
    fst::SymbolTable *word_syms = NULL;
    fst::Fst<fst::StdArc> *decode_fst = NULL;
    
    retval = get_online_python_gmm_decode_faster(argc, argv, &decoder, &decodable,
                &feat_transform, &word_syms, &decode_fst);
    if(retval != 0) {
        printf("Error in get_online_python_gmm_decode_faster. Return code %d\n", retval);
        if (retval == 1) {
          printf("Just wrong commandline arguments. Return code %d\n", retval);
          return 0;
        }
        return retval;
    }

    // start the decoding
    retval = decode(decoder, decodable, word_syms);
    if(retval != 0) {
        printf("Error in decode. Return code %d\n", retval);
        return retval;
    }

    // tidy up
    if (decoder) delete decoder;
    if (decodable) delete decodable;
    if (feat_transform) delete feat_transform;
    if (word_syms) delete word_syms;
    if (decode_fst) delete decode_fst;

    dlclose(lib);
    return 0;
}
