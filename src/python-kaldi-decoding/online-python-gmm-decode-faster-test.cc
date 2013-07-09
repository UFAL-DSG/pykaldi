#include "online-python-gmm-decode-faster.h"

int main(int argc, char **argv) {
    // FIXME do not use the decoder via command line argumets
    // FIXME implement real test which should simulate the calls from Python
    // without arguments it returns return code 1 -> we want 0 

    void *lib = dlopen("libpykaldi.so", RTLD_NOW);
    if (!lib) {
        printf("Cannot open library: %s\n", dlerror());
        return 1;
    }   
    char [] nameFce;

    dlerror();  // reset errors
    nameFce = "get_online_python_gmm_decode_faster";
    get_decoder_t get_online_python_gmm_decode_faster = (get_decoder_t)dlsym(lib, nameFce); 
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    dlerror();  // reset errors
    nameFce = "decode"
    decode_t decode = (get_decoder_t)dlsym(lib, nameFce); 
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    // Using the functions
    int retval;

    // get the decoder and other objects needed 
    OnlineFasterDecoder *decoder, 
    OnlineDecodableDiagGmmScaled *decodable, 
    OnlineFeatInputItf *feat_transform, 
    fst::SymbolTable *word_syms,
    fst::Fst<fst::StdArc> *decode_fst;
    
    retval = get_online_python_gmm_decode_faster(argc, argv, decoder, decodable,
                feat_transform, word_syms, decode_fst);
    if(retval != 0) {
        printf("Error in get_online_python_gmm_decode_faster. Return code %d", retval);
        return 1;
    }

    // start the decoding
    retval = decode(decoder, decodable, word_syms);
    if(retval != 0) {
        printf("Error in decode. Return code %d", retval);
        return 1;
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
