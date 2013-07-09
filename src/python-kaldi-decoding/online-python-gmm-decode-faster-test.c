#include "test-cffi-python-dyn.h"

int main(int argc, char **argv) {
    // FIXME do not use the decoder via command line argumets
    // FIXME implement real test which should simulate the calls from Python
    // without arguments it returns return code 1 -> we want 0 
    return testSharedLibS(argc, argv, "online_python_gmm_decode_faster_like_main") -1;
}
