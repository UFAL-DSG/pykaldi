#include "test-cffi-python-dyn.h"

int main(int argc, char **argv) {
    return testSharedLibS(argc, argv, "compute_wer_like_main") -1;
}
