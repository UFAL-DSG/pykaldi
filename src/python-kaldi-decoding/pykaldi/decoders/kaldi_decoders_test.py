#!/usr/bin/env python
from kaldiDecoders import NbListDecoder
import unittest


class TestDecoders(unittest.TestCase):

    def setUp(self):
        self.data = 'my testing string'
        self.samples_per_frame = 256
        import pyaudio
        self.p = pyaudio.PyAudio()
        self.stream = self.p.open(format=self.p.get_format_from_width(pyaudio.paInt32),
                                  channels=1,
                                  rate=16000,
                                  input=True,
                                  output=True,
                                  frames_per_buffer=self.samples_per_frame)

    def test_nblist(self):
        frame_payload = self.stream.read(self.samples_per_frame)
        d = NbListDecoder()
        d.rec_in(frame_payload)
        hyp_list = d.decode()
        self.assertIsInstance(hyp_list, list)

    def tearDown(self):
        self.stream.stop_stream()
        self.stream.close()
        self.p.terminate()
        self.p, self.stream = None, None

if __name__ == '__main__':
    unittest.main()
