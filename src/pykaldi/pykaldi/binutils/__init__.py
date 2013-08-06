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
    from cffi import FFI, VerificationError
except ImportError as e:
    print '''

For running pykaldi you need cffi module installed!

'''
    raise e


def init_bin():
    ffibin = FFI()
    ffibin.cdef('int compute_wer_like_main(int argc, char **argv);')

    srcdir = os.path.realpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../..'))
    decwrapdir = os.path.join(srcdir, 'dec-wrap')
    libs = ['pykaldi']
    try:
        libbin = ffibin.verify(
            '#include "dec-wrap/compute-wer.h"',
            libraries=libs,
            include_dirs=[srcdir],
            library_dirs=[decwrapdir],
            runtime_library_dirs=[decwrapdir],
            ext_package='pykaldi',
        )
    except VerificationError as e:
        print 'Have you compiled libraries: %s?' % str(libs)
        raise e
    return (ffibin, libbin)

ffibin, libbin = init_bin()
