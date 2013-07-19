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


import pykaldi
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
int compute_wer_like_main(int argc, char **argv);
'''
ffibin.cdef(binheader)
dir_path = os.path.dirname(os.path.realpath(pykaldi.__file__))
lib_name = 'libpykaldi.so'
shared_lib_path = os.path.join(dir_path, lib_name)
try:
    libbin = ffibin.dlopen(shared_lib_path)
except OSError as e:
    print 'Could not find the C shared library %s' % shared_lib_path
    raise e
