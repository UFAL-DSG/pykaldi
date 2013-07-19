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

# FIXME I think that using add_ld_path(dir_path) is not necessary!
# def add_ld_library_path(newdir):
#     lib_path = os.environ.get('LD_LIBRARY_PATH', '')
#     if not lib_path:
#         os.environ['LD_LIBRARY_PATH'] = newdir
#     else:
#         os.environ['LD_LIBRARY_PATH'] = newdir + os.pathsep + lib_path
#
# add_ld_library_path(dir_path)


def init_dec():
    ffidec = FFI()
    ffidec.cdef('''
    typedef ... CKaldiDecoderWrapper;

    CKaldiDecoderWrapper* new_KaldiDecoderWrapper();
    void del_KaldiDecoderWrapper(CKaldiDecoderWrapper *d);

    size_t Decode(CKaldiDecoderWrapper *d);
    size_t HypSize(void);
    size_t FinishDecoding(CKaldiDecoderWrapper *d);
    bool Finished(void);
    void FrameIn(CKaldiDecoderWrapper *d, unsigned char *frame, size_t frame_len);
    void PopHyp(CKaldiDecoderWrapper *d, int * word_ids, size_t size);
    size_t PrepareHypothesis(CKaldiDecoderWrapper *d, int * is_full);
    void Reset(CKaldiDecoderWrapper *d);
    int Setup(CKaldiDecoderWrapper *d, int argc, char **argv);

    ''')

    dir_path = os.path.dirname(os.path.realpath(__file__))
    lib_name = 'libpykaldi.so'
    shared_lib_path = os.path.join(dir_path, lib_name)

    try:
        libdec = ffidec.dlopen(shared_lib_path)
    except OSError as e:
        print 'Could not find the C shared library %s' % shared_lib_path
        raise e
    return (ffidec, libdec)


ffidec, libdec = init_dec()
