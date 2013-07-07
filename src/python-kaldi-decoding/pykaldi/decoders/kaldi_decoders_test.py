#!/usr/bin/env python
from kaldi_decoders import NbListDecoder
import unittest
import audioop
import wave
# Just import this is a test ;-)
from pykaldi.decoders import ffidec as ffi, libdec as lib


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


class TestAudio(unittest.TestCase):
    def setUp(self):
        self.wav_path = 'test.wav'
        self.pyaudio_loaded = False
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

    def setup_pyaudio(self):
        import pyaudio
        self.p = pyaudio.PyAudio()
        self.stream = self.p.open(format=self.p.get_format_from_width(pyaudio.paInt32),
                                  channels=1,
                                  rate=16000,
                                  input=True,
                                  output=True,
                                  frames_per_buffer=self.samples_per_frame)
        self.pyaudio_loaded = True

    def test_setup_pyaudio(self):
        self.setup_pyaudio()

    def teardown_pyaudio(self):
        self.stream.stop_stream()
        self.stream.close()
        self.p.terminate()
        self.p, self.stream, self.pyaudio_loaded = None, None, False

    def tearDown(self):
        if self.pyaudio_loaded:
            self.teardown_pyaudio()


class TestDecoders(unittest.TestCase):
    def setUp(self):
        self.data = 'my testing string'

    # Currently we not at phase to decode anything real
    # def test_nblist(self):
    #     self.setup_pyaudio()
    #     frame_payload = self.stream.read(self.samples_per_frame)
    #     d = NbListDecoder()
    #     d.rec_in(frame_payload)
    #     hyp_list = d.decode()
    #     self.assertIsInstance(hyp_list, list)

    def test_nblist_wav(self):
        d = NbListDecoder()
        d.rec_in(self.data)
        hyp_list = d.decode()
        self.assertIsInstance(hyp_list, list)

    def tearDown(self):
        pass

if __name__ == '__main__':
    unittest.main()


def load_wav(file_name, def_sample_rate=16000):
    """
    Source: from Alex/utils/audio.py
    Reads all audio data from the file and returns it in a string.

    The content is re-sampled into the default sample rate.

    """
    try:
        wf = wave.open(file_name, 'r')
        if wf.getnchannels() != 1:
            raise Exception('Input wave is not in mono')

        if wf.getsampwidth() != 2:
            raise Exception('Input wave is not in 16bit')

        sample_rate = wf.getframerate()

        # read all the samples
        chunk = 1024
        pcm = b''
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
