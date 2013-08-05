"""
The module wraps C++/Python interface for Kaldi decoders.
"""
# Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
# WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
# MERCHANTABLITY OR NON-INFRINGEMENT.
# See the Apache 2 License for the specific language governing permissions and
# limitations under the License. #

import os
try:
    from cffi import FFI
except ImportError as e:
    print '''

For running pykaldi you need cffi module installed!

'''
    raise e


def init_audio():
    ffiaudio = FFI()
    ffiaudio.cdef('''

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
    lib_name = 'libaudio.so'
    shared_lib_path = os.path.join(dir_path, lib_name)
    try:
        libaudio = ffiaudio.dlopen(shared_lib_path)
    except OSError as e:
        print 'Could not find the C shared library %s' % shared_lib_path
        print 'OR SOME SYMBOLS IN LIBRARY ARE UNREFERENCED'
        raise e
    return (ffiaudio, libaudio)


ffiaudio, libaudio = init_audio()
