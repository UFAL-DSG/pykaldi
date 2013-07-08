#include "test-cffi-python-dyn.h"

int main(int argc, char **argv) {
    return testSharedLibS(argc, argv, "compute_cmvn_stats_like_main") -1;
}
