#include "test-cffi-python-dyn.h"

int main(int argc, char **argv) {
    return testSharedLib("libkaldi-cffi.so", "online_wav_gmm_decode_faster_like_main", argc, argv);
}
