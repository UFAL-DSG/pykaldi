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
# Just import this is a test ;-)
from pykaldi.decoders import ffidec as ffi, libdec as lib


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
        self.wav_path = 'test.wav'
        # TODO fix arguments for online decoder
        self.argv = [
            '--rt-min=0.5', '--rt-max=0.7', '--max-active=4000', '--beam=12.0', '--acoustic-scale=0.0769', '~/50GBmax/kaldi/egs/voxforge/online_demo/online-data/models/tri2b_mmi/model', '~/50GBmax/kaldi/egs/voxforge/online_demo/online-data/models/tri2b_mmi/HCLG.fst',
            '~/50GBmax/kaldi/egs/voxforge/online_demo/online-data/models/tri2b_mmi/words.txt', '1:2:3:4:5', '~/50GBmax/kaldi/egs/voxforge/online_demo/online-data/models/tri2b_mmi/matrix']
        self.samples_per_frame = 256
        self.pyaudio_loaded = False
        self.p, self.stream = setup_pyaudio(self.samples_per_frame)  # may throw Exception
        self.pyaudio_loaded = True

    def test_wav(self, decode_once=True, hyp_once=True):
        with OnlineDecoder(self.argv) as d:
            pcm = load_wav(self.wav_path)
            play_len, frame_len = len(pcm), self.samples_per_frame
            for i in range(play_len / frame_len):
                frame = pcm[i * frame_len:(i + 1) * frame_len]
                d.frame_in(frame)
                if not decode_once:
                    d.decode()
                if not hyp_once:
                    size, full_hyp = d.prepare_hyp()
                    prop, hyp = d.get_hypothesis(size)
                    print size, str(hyp)
            d.decode()
            size, full_hyp = d.prepare_hyp()
            prop, hyp = d.get_hypothesis(size)
            print size, str(hyp)

    def test_wav_decode_often(self):
        self.test_wav(decode_once=False)

    def test_wav_hyp_often(self):
        self.test_wav(decode_once=False, hyp_once=True)

    def test_wav_hyp_too_often(self):
        self.test_wav(decode_once=True, hyp_once=False)

    def test_live(self, duration=3):
        with OnlineDecoder(self.argv) as d:
            for i in xrange(duration * (16000 / self.samples_per_frame)):
                frame = self.stream.read(self.samples_per_frame)
                d.frame_in(frame)
                d.decode()
            size, full_hyp = d.prepare_hyp()
            print 'Hypothesis is full %d' % full_hyp
            prop, hyp = d.get_hypothesis(size)
            print "probability: %d" % prop
            self.assertEqual(prop, 1.0, 'Is probability measure implemented now?')
            print 'Numpy word ids: %s' % str(hyp)
            self.assertTrue(len(hyp) > 0, 'We should decode something if you speak!')

    def tearDown(self):
        if self.pyaudio_loaded:
            teardown_pyaudio(self.p, self.stream)


class TestAudio(unittest.TestCase):
    def setUp(self):
        self.wav_path = 'test.wav'
        self.samples_per_frame = 256

    def test_play_list(self):
        pcm = load_wav(self.wav_path)
        play_len, frame_len = len(pcm), self.samples_per_frame

        flp = lib.create_frame_list(2, frame_len)
        keep_alive = []
        for i in range(play_len / frame_len):
            frame = pcm[i * frame_len:(i + 1) * frame_len]
            keep_alive.append(frame)
            lib.add_frame_to_list(flp, frame)
        handlep = lib.play_setup()
        if handlep == ffi.NULL:
            raise OSError("Alsa not initialized")
        # using two chars for one sample of 16 bit audio is handled by play_list itself!
        ret_code = lib.play_list(handlep, flp)
        self.assertEqual(
            ret_code, 0, 'Function play from C library FAILED with exit status %s' % ret_code)
        lib.play_tear_down(handlep)
        lib.delete_frame_list(flp)

    def test_play(self):
        pcm = load_wav(self.wav_path)
        play_len = len(pcm) / 2  # using two chars for one sample of 16 bit audio
        handlep = lib.play_setup()
        if handlep == ffi.NULL:
            raise OSError("Alsa not initialized")
        ret_code = lib.play(handlep, pcm, play_len)
        self.assertEqual(
            ret_code, 0, 'Function play from C library FAILED with exit status %s' % ret_code)
        lib.play_tear_down(handlep)


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
        flp = lib.create_frame_list(2, 1)
        for c in s:
            # pcm is stored like char array:  2 chars == one 16bit sample
            lib.add_frame_to_list(flp, c)

        # C pointer magic: i is in range of (end_pointer - start_pointer)
        start = lib.frame_list_start(flp)
        for i in range(lib.frame_list_end(flp) - start):
            it = start + i
            # casting unsigned char** to char**
            str_p = ffi.cast("char **", it)
            # Dereferencing in cffi way p[0] instead of *p
            c_str = ffi.string(str_p[0])
            python_str = s[i]
            self.assertEqual(c_str, python_str,
                             'Failed at %d th string: %s vs %s!'
                             % (i, c_str, python_str))

        lib.delete_frame_list(flp)


if __name__ == '__main__':
    unittest.main()
