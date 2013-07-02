#include "test-cffi-python-dyn.h"

int main(int argc, char **argv) {
    // without arguments it returns return code 1 -> we want 0 
    return testSharedLibS(argc, argv, "lattice_best_path_like_main") -1;
}
