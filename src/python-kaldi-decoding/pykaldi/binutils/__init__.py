try:
    from cffi import FFI
except ImportError as e:
    print '''

For running pykaldi you need cffi module installed!

'''
    raise e

# binary like utils from Kaldi

binffi = FFI()
binheader = '''
int compute_mfcc_feats_like_main(int argc, char **argv);
int gmm_latgen_faster_like_main(int argc, char **argv);
int lattice_best_path_like_main(int argc, char **argv);
int compute_wer_like_main(int argc, char **argv);
int online_wav_gmm_decode_faster_like_main(int argc, char *argv[]);
'''
binffi.cdef(binheader)
bin_lib_path = 'libpykaldi.so'
try:
    binlib = binffi.dlopen(bin_lib_path)
except OSError as e:
    print 'Could not find the C shared library %s' % bin_lib_path
    raise e
