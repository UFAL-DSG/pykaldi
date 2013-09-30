#!/usr/bin/env python
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


from pykaldi.binutils import ffibin, libbin
import sys
from pykaldi.exceptions import PyKaldiError

if __name__ == '__main__':
    '''Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/local/score.sh | compute-wer --text --mode=present
    exp/tri2a/decode/wer_15 ark:exp/tri2a/decode/scoring/test_filt.txt
    ark,p:- >& exp/tri2a/decode/wer_15'''
    wer_argkeepalive = [ffibin.new("char[]", arg) for arg in sys.argv]
    wer_argv = ffibin.new("char *[]", wer_argkeepalive)
    retcode = libbin.compute_wer_like_main(len(wer_argkeepalive), wer_argv)
    if retcode != 0:
        raise PyKaldiError(retcode)
