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


from pykaldi.decoders import ffidec, libdec
from pykaldi.decoders import ffidummy, libdummy
import numpy


class KaldiDecoder(object):
    """Python wrapper for accessing C Kaldi decs.
    """

    def __init__(self, fs=16000, nchan=1):
        """@todo: Docstring for __init__
        :fs: Sampling frequency
        :nchan: Number of channels
        """
        # FIXME send the settings to the Recorder or VERIFY them here
        self.fs = fs
        self.nchan = nchan
        # This has to set in the derived classes
        self.lib, self.ffi, self.dec = None, None, None

    def rec_in(self, frame):
        """Recieving audio frame by frame, so defining asynchronous API.
        The input API is the same for all decs.
        :frame: @todo
        :returns: @todo
        """
        frame_p = self.ffi.new('char[]', frame)
        # frame_in has to memcpy the frame out
        # because frame_p can be deallocated after leaving this method
        self.lib.frame_in(self.dec, frame_p, len(frame))


class DummyDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decs."""

    def __init__(self, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.lib, self.ffi = libdummy, ffidummy
        self.dec = self.ffi.new('char []', 'unused')

    def decode(self):
        """Returns nblist
        :returns: list of tuples (double-probability, string-sentence)
        """
        prob_p, ans_p = self.ffi.new('double *'), self.ffi.new('char **')
        ans_size = self.ffi.new('size_t *')
        self.lib.return_answer(prob_p, ans_p, ans_size)
        ans, prob = self.ffi.string(ans_p[0], ans_size[0]), prob_p[0]
        return [(prob, ans)]


class OnlineDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decs."""

    def __init__(self, argv, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.lib, self.ffi = libdec, ffidec
        # necessary to keep it alive long enough -> member field-> ok
        self.argv = [self.ffi.new("char[]", arg) for arg in argv]
        self.dec = self.__enter__()

    def __enter__(self):
        argc, argp = len(self.argv), self.ffi.new("char *[]", self.argv)
        dec_pointer = self.lib.new_KaldiDecoderWrapper(argc, argp)
        return dec_pointer

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.lib.del_KaldiDecoderWrapper(self.dec)

    def close(self):
        self.__exit__(None, None, None)

    def frame_in(self, frame_str):
        self.lib.FrameIn(self.dec, frame_str, len(frame_str))

    def decode(self):
        """Ask the dec to process the buffered data.
        Does not return any output.
        """
        self.lib.Decode(self.dec)

    def prepare_hyp(self):
        full_hyp_p = self.ffi.new("int *")
        size = self.lib.PrepareHypothesis(self.dec, full_hyp_p)
        # TODO should I convert size?
        return (size, full_hyp_p[0])

    def get_hypothesis(self, size):
        # TODO our dec does not return any measure of quality for the decoded hypothesis
        # prob_p = self.ffi.new('double *')
        hyp = numpy.zeros(size).astype('int32')
        hyp_p = self.ffi.cast("int *", hyp.ctypes.data)
        self.lib.GetHypothesis(self.dec, hyp_p, size)
        prob = 1.0  # TODO prob = prob_p[0]  # TODO
        return (prob, hyp)


class ConfNetDecoder(KaldiDecoder):
    """Docstring for ConfNetDecoder """

    def __init__(self):
        """@todo: to be defined """
        pass
