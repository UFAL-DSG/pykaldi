#include "test_cffi_python_dyn.h"

int main(int argc, char **argv) {
    return testSharedLib("libcffi-kaldi.so", "lattice_best_path_like_main", argc, argv);
}
