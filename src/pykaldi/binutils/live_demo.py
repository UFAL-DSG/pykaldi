import pyaudio
from pykaldi.decoders import OnlineDecoder
import sys


def setup_pyaudio(samples_per_frame):
    p = pyaudio.PyAudio()
    stream = p.open(format=p.get_format_from_width(pyaudio.paInt32),
                    channels=1,
                    rate=16000,
                    input=True,
                    output=True,
                    frames_per_buffer=samples_per_frame)
    return (p, stream)


def teardown_pyaudio(p, stream):
    stream.stop_stream()
    stream.close()
    p.terminate()
    p, stream = None, None


def run_live(self, stream, duration=3):
    # FIXME is broken
    d = OnlineDecoder(self.argv)
    for i in xrange(duration * (16000 / self.samples_per_frame)):
        frame = stream.read(2 * self.samples_per_frame)
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

if __name__ == '__main__':
    samples_per_frame = 2120
    p, stream = setup_pyaudio(sys.args)
    run_live(stream)
    teardown_pyaudio(p, stream)
