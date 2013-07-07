from pykaldi.decoders import ffidec, libdec
import os




class KaldiDecoder(object):
    """Python wrapper for accessing C Kaldi decoders.
    """

    def __init__(self, fs=16000, nchan=1):
        """@todo: Docstring for __init__
        :fs: Sampling frequency
        :nchan: Number of channels
        """
        # FIXME send the settings to the Recorder or VERIFY them here
        self.fs = fs
        self.nchan = nchan

    def rec_in(self, frame):
        """Recieving audio frame by frame, so defining asynchronous API.
        The input API is the same for all decoders.
        :frame: @todo
        :returns: @todo
        """
        frame_p = ffidec.new('char[]', frame)
        # frame_in has to memcpy the frame out
        # because frame_p can be deallocated after leaving this method
        libdec.frame_in(frame_p, len(frame))


class NbListDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decoders."""

    def __init__(self, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)

    def decode(self):
        """Returns nblist
        :returns: list of tuples (double-probability, string-sentence)

        """
        prob_p, ans_p, ans_size = ffidec.new(
            'double *'), ffidec.new('char **'), ffidec.new('size_t *')
        libdec.return_answer(prob_p, ans_p, ans_size)
        ans, prob = ffidec.string(ans_p[0], ans_size[0]), prob_p[0]
        return [(prob, ans)]


class ConfNetDecoder(KaldiDecoder):
    """Docstring for ConfNetDecoder """

    def __init__(self):
        """@todo: to be defined """
        pass
