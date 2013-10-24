import unittest
from pykaldi.utils import load_wav
from decoders import PyGmmLatgenWrapper, DecoderCloser


class PyGmmLatgenWrapperFucnTest(unittest.TestCase):

    def setUp(self):
        dir_path = os.path.realpath(os.path.dirname(__file__))
        self.wav_path = os.path.join(dir_path, 'audio', 'test.wav')
        get_voxforge_data(path=dir_path)
        p = os.path.join(dir_path, 'online-data', 'models', 'tri2a')
        self.argv = ['--verbose=0', '--max-active=4000', '--beam=12.0',
                     '--acoustic-scale=0.0769',
                     os.path.join(p, 'model'),
                     os.path.join(p, 'HCLG.fst'),
                     os.path.join(p, 'words.txt'),
                     '1:2:3:4:5']

    def test_wav(self, words_to_dec=3):
        pcm = load_wav(self.wav_path)
        # Test PykaldiFasterDecoder
        samples_per_frame = 2120
        word_ids, prob = run_online_dec(pcm, self.argv, samples_per_frame)
        print 'From %s decoded %d utt: %s' % (self.wav_path, len(word_ids), str(word_ids))
        self.assertTrue(len(word_ids) > words_to_dec,
                        'We have to decode at least %d words' % words_to_dec)
        self.assertAlmostEqual(
            prob, 1.0, 'Have we implemented the probability or sthing wrong returned')


if __name__ == '__main__':
    unittest.main()
