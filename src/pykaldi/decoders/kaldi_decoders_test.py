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


from kaldi_decoders import DummyDecoder, OnlineDecoder
import unittest
import audioop
import wave
import os
# Just import this is a test ;-)
from pykaldi.decoders import ffidummy, libdummy


def run_online_dec(pcm, argv, samples_per_frame):
    d = OnlineDecoder(argv)
    play_len, total_words = len(pcm), 0
    # using 16-bit audio so 1 sample = 2 chars
    frame_len = (2 * samples_per_frame)
    for i in range(play_len / frame_len):
        frame = pcm[i * frame_len:(i + 1) * frame_len]
        d.frame_in(frame, samples_per_frame)

    d.finish_input()
    while d.decode():
        num_words, full_hyp = d.prepare_hyp()
        prop, word_ids = d.get_hypothesis(num_words)
        print 'full: %r, num_words %d, ids: %s' % (full_hyp, num_words, str(word_ids))
        total_words += num_words
    print 'Decode last hypothesis'
    num_words, full_hyp = d.prepare_hyp()
    prop, word_ids = d.get_hypothesis(num_words)
    print 'full: %r, num_words %d, ids: %s' % (full_hyp, num_words, str(word_ids))
    total_words += num_words
    print 'Total words: ', total_words
    # DO NOT FORGET TO CLOSE THE DECODER!
    d.close()


def teardown_pyaudio(p, stream):
    stream.stop_stream()
    stream.close()
    p.terminate()
    p, stream = None, None


def setup_pyaudio(samples_per_frame):
    import pyaudio
    p = pyaudio.PyAudio()
    stream = p.open(format=p.get_format_from_width(pyaudio.paInt32),
                    channels=1,
                    rate=16000,
                    input=True,
                    output=True,
                    frames_per_buffer=samples_per_frame)
    return (p, stream)


def load_wav(file_name, def_sample_rate=16000):
    """ Source: from Alex/utils/audio.py
    Reads all audio data from the file and returns it in a string.

    The content is re-sampled into the default sample rate."""
    try:
        wf = wave.open(file_name, 'r')
        if wf.getnchannels() != 1:
            raise Exception('Input wave is not in mono')
        if wf.getsampwidth() != 2:
            raise Exception('Input wave is not in 16bit')
        sample_rate = wf.getframerate()
        # read all the samples
        chunk, pcm = 1024, b''
        pcmPart = wf.readframes(chunk)
        while pcmPart:
            pcm += str(pcmPart)
            pcmPart = wf.readframes(chunk)
    except EOFError:
        raise Exception('Input PCM is corrupted: End of file.')
    else:
        wf.close()
    # resample audio if not compatible
    if sample_rate != def_sample_rate:
        pcm, state = audioop.ratecv(pcm, 2, 1, sample_rate, def_sample_rate, None)

    return pcm


class TestOnlineDecoder(unittest.TestCase):
    def setUp(self):
        dir_path = os.path.realpath(os.path.dirname(__file__))
        # Should be extracted by makefile
        self.wav_path = dir_path + '/online-data/audio/test1.wav'
        # self.wav_path = 'test.wav'
        p = dir_path + '/online-data/models/tri2b_mmi'
        self.argv = [
            '--rt-min=0.5', '--rt-max=99.0', '--max-active=4000', '--beam=12.0',
            '--acoustic-scale=0.0769', '%s/model' % p, '%s/HCLG.fst' % p,
            '%s/words.txt' % p, '1:2:3:4:5', '%s/matrix' % p]
        self.samples_per_frame = 2120
        self.pyaudio_loaded = False

    def _setup_pyaudio(self):
        self.p, self.stream = setup_pyaudio(self.samples_per_frame)  # throw Exception
        self.pyaudio_loaded = True

    def test_wav(self):
        pcm = load_wav(self.wav_path)
        run_online_dec(pcm, self.argv, self.samples_per_frame)

    @unittest.skip("Do not run live test by default")
    def test_live(self, duration=3):
        # FIXME is broken
        self._setup_pyaudio()
        d = OnlineDecoder(self.argv)
        for i in xrange(duration * (16000 / self.samples_per_frame)):
            frame = self.stream.read(2 * self.samples_per_frame)
            d.frame_in(frame, len(frame) * 2)
            while d.decode():
                num_words, full_hyp = d.prepare_hyp()
                prop, word_ids = d.get_hypothesis(num_words)
                print 'full: %r, num_words %d, ids: %s' % (full_hyp, num_words, str(word_ids))
        print 'Decode last hypothesis'
        num_words, full_hyp = d.prepare_hyp()
        prop, word_ids = d.get_hypothesis(num_words)
        print 'full: %r, num_words %d, ids: %s' % (full_hyp, num_words, str(word_ids))
        self.assertEqual(prop, 1.0, 'Is probability measure implemented now?')
        d.close()

    def tearDown(self):
        if self.pyaudio_loaded:
            teardown_pyaudio(self.p, self.stream)


class TestAudio(unittest.TestCase):
    def setUp(self):
        self.wav_path = 'test.wav'
        self.samples_per_frame = 256

    @unittest.skip("Requires ALSA library (on Linux). Do not run live test by default")
    def test_play_list(self):
        pcm = load_wav(self.wav_path)
        play_len, frame_len = len(pcm), self.samples_per_frame

        flp = libdummy.create_frame_list(2, frame_len)
        keep_alive = []
        for i in range(play_len / frame_len):
            frame = pcm[i * frame_len:(i + 1) * frame_len]
            keep_alive.append(frame)
            libdummy.add_frame_to_list(flp, frame)
        handlep = libdummy.play_setup()
        if handlep == ffidummy.NULL:
            raise OSError("Alsa not initialized")
        # using two chars for one sample of 16 bit audio is handled by play_list itself!
        ret_code = libdummy.play_list(handlep, flp)
        self.assertEqual(
            ret_code, 0, 'Function play from C libdummyrary FAILED with exit status %s' % ret_code)
        libdummy.play_tear_down(handlep)
        libdummy.delete_frame_list(flp)

    @unittest.skip("Requires ALSA library (on Linux). Do not run live test by default")
    def test_play(self):
        pcm = load_wav(self.wav_path)
        play_len = len(pcm) / 2  # using two chars for one sample of 16 bit audio
        handlep = libdummy.play_setup()
        if handlep == ffidummy.NULL:
            raise OSError("Alsa not initialized")
        ret_code = libdummy.play(handlep, pcm, play_len)
        self.assertEqual(
            ret_code, 0, 'Function play from C libdummyrary FAILED with exit status %s' % ret_code)
        libdummy.play_tear_down(handlep)


class TestDummyDecodder(unittest.TestCase):
    def setUp(self):
        self.data = 'my testing string'

    def test_nblist_wav(self):
        d = DummyDecoder()
        d.rec_in(self.data)
        hyp_list = d.decode()
        self.assertIsInstance(hyp_list, list)

    def tearDown(self):
        pass


class TestFrames(unittest.TestCase):
    def setUp(self):
        import string
        self.test_str = string.ascii_lowercase

    def test_list(self):
        s = self.test_str
        # store strings of length 1
        flp = libdummy.create_frame_list(2, 1)
        for c in s:
            # pcm is stored like char array:  2 chars == one 16bit sample
            libdummy.add_frame_to_list(flp, c)

        # C pointer magic: i is in range of (end_pointer - start_pointer)
        start = libdummy.frame_list_start(flp)
        for i in range(libdummy.frame_list_end(flp) - start):
            it = start + i
            # casting unsigned char** to char**
            str_p = ffidummy.cast("char **", it)
            # Dereferencing in cffidummy way p[0] instead of *p
            c_str = ffidummy.string(str_p[0])
            python_str = s[i]
            self.assertEqual(c_str, python_str,
                             'Failed at %d th string: %s vs %s!'
                             % (i, c_str, python_str))

        libdummy.delete_frame_list(flp)


if __name__ == '__main__':
    unittest.main()
