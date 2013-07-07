'''
The module interface C++ Kaldi decoders from Python.
'''
import os
try:
    from cffi import FFI
except ImportError as e:
    print '''

For running pykaldi you need cffi module installed!

'''
    raise e


ffidec = FFI()
ffidec.cdef('''
void return_answer(double * prob, char **ans, size_t *size);
void frame_in(char *str_frame, size_t size);

typedef ... snd_pcm_t;
int play(snd_pcm_t *handle, unsigned char * buffer, size_t size);
snd_pcm_t * play_setup(void);
void play_tear_down(snd_pcm_t *handle);

typedef ... frame_list;
frame_list * create_frame_list(size_t block_size, size_t frame_size);
void delete_frame_list(frame_list * pfl);
int add_frame_to_list(frame_list * pfl, unsigned char * frame);
unsigned char ** frame_list_start(frame_list *pfl);
unsigned char ** frame_list_end(frame_list *pfl);

int play_list(snd_pcm_t *handle, frame_list * fl);
''')

dir_path = os.path.dirname(os.path.realpath(__file__))
lib_name = 'libdummyio.so'
shared_lib_path = os.path.join(dir_path, lib_name)

# FIXME I think that using add_ld_path(dir_path) is not necessary!
# def add_ld_library_path(newdir):
#     lib_path = os.environ.get('LD_LIBRARY_PATH', '')
#     if not lib_path:
#         os.environ['LD_LIBRARY_PATH'] = newdir
#     else:
#         os.environ['LD_LIBRARY_PATH'] = newdir + os.pathsep + lib_path
#
# add_ld_library_path(dir_path)

try:
    libdec = ffidec.dlopen(shared_lib_path)
except OSError as e:
    print 'Could not find the C shared library %s' % shared_lib_path
    raise e
