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


import unittest
import os
# Just import this is a test ;-)
from pykaldi.binutils.utils import load_wav
from pykaldi.binutils.online_decode import run_online_dec


class TestOnlineDecoder(unittest.TestCase):
    def setUp(self):
        dir_path = os.path.realpath(os.path.dirname(__file__))
        # Should be extracted by makefile
        self.wav_path = 'audio/test.wav'
        # self.wav_path = dir_path + '/online-data/audio/test1.wav'
        p = dir_path + '/online-data/models/tri2a'
        self.argv = [
            '--rt-min=0.5', '--rt-max=99.0', '--max-active=4000', '--beam=12.0',
            '--acoustic-scale=0.0769', '%s/model' % p, '%s/HCLG.fst' % p,
            '%s/words.txt' % p, '1:2:3:4:5']
        self.samples_per_frame = 2120

    def test_wav(self, words_to_dec=3):
        pcm = load_wav(self.wav_path)
        # Test OnlineDecoder
        word_ids, prob = run_online_dec(pcm, self.argv, self.samples_per_frame)
        print 'From %s decoded %d utt: %s' % (self.wav_path, len(word_ids), str(word_ids))
        self.assertTrue(len(word_ids) > words_to_dec,
                        'We have to decode at least %d words' % words_to_dec)
        self.assertAlmostEqual(prob, 1.0, 'Have we implemented the probability or sthing wrong returned')


if __name__ == '__main__':
    unittest.main()
