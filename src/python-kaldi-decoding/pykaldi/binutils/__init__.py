import os
try:
    from cffi import FFI
except ImportError as e:
    print '''

For running pykaldi you need cffi module installed!

'''
    raise e

# binary like utils from Kaldi

ffibin = FFI()
binheader = '''
int compute_mfcc_feats_like_main(int argc, char **argv);
int gmm_latgen_faster_like_main(int argc, char **argv);
int lattice_best_path_like_main(int argc, char **argv);
int compute_wer_like_main(int argc, char **argv);
int online_wav_gmm_decode_faster_like_main(int argc, char *argv[]);
int compute_cmvn_stats_like_main(int argc, char *argv[]);
'''
ffibin.cdef(binheader)
dir_path = os.path.dirname(os.path.realpath(__file__))
lib_name = 'libpykaldi.so'
shared_lib_path = os.path.join(dir_path, lib_name)
try:
    libbin = ffibin.dlopen(shared_lib_path)
except OSError as e:
    print 'Could not find the C shared library %s' % shared_lib_path
    raise e
