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
from pykaldi.audio import libaudio, ffiaudio
from pykaldi.binutils.utils import load_wav


class TestFrames(unittest.TestCase):
    def setUp(self):
        import string
        self.test_str = string.ascii_lowercase

    def test_list(self):
        s = self.test_str
        # store strings of length 1
        flp = libaudio.create_frame_list(2, 1)
        for c in s:
            # pcm is stored like char array:  2 chars == one 16bit sample
            libaudio.add_frame_to_list(flp, c)

        # C pointer magic: i is in range of (end_pointer - start_pointer)
        start = libaudio.frame_list_start(flp)
        for i in range(libaudio.frame_list_end(flp) - start):
            it = start + i
            # casting unsigned char** to char**
            str_p = ffiaudio.cast("char **", it)
            # Dereferencing in cffiaudio way p[0] instead of *p
            c_str = ffiaudio.string(str_p[0])
            python_str = s[i]
            self.assertEqual(c_str, python_str,
                             'Failed at %d th string: %s vs %s!'
                             % (i, c_str, python_str))

        libaudio.delete_frame_list(flp)


class TestAudio(unittest.TestCase):
    def setUp(self):
        self.wav_path = 'test.wav'
        self.samples_per_frame = 256

    @unittest.skip("Requires ALSA library (on Linux). Do not run live test by default")
    def test_play_list(self):
        pcm = load_wav(self.wav_path)
        play_len, frame_len = len(pcm), self.samples_per_frame

        flp = libaudio.create_frame_list(2, frame_len)
        keep_alive = []
        for i in range(play_len / frame_len):
            frame = pcm[i * frame_len:(i + 1) * frame_len]
            keep_alive.append(frame)
            libaudio.add_frame_to_list(flp, frame)
        handlep = libaudio.play_setup()
        if handlep == ffiaudio.NULL:
            raise OSError("Alsa not initialized")
        # using two chars for one sample of 16 bit audio is handled by play_list itself!
        ret_code = libaudio.play_list(handlep, flp)
        self.assertEqual(
            ret_code, 0, 'Function play from C libaudiorary FAILED with exit status %s' % ret_code)
        libaudio.play_tear_down(handlep)
        libaudio.delete_frame_list(flp)

    @unittest.skip("Requires ALSA library (on Linux). Do not run live test by default")
    def test_play(self):
        pcm = load_wav(self.wav_path)
        play_len = len(pcm) / 2  # using two chars for one sample of 16 bit audio
        handlep = libaudio.play_setup()
        if handlep == ffiaudio.NULL:
            raise OSError("Alsa not initialized")
        ret_code = libaudio.play(handlep, pcm, play_len)
        self.assertEqual(
            ret_code, 0, 'Function play from C libaudiorary FAILED with exit status %s' % ret_code)
        libaudio.play_tear_down(handlep)


if __name__ == '__main__':
    unittest.main()
