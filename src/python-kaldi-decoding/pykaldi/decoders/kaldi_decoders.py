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
        # This has to set in the derived classes
        self.clib = None
        self.decoder = None

    def rec_in(self, frame):
        """Recieving audio frame by frame, so defining asynchronous API.
        The input API is the same for all decoders.
        :frame: @todo
        :returns: @todo
        """
        frame_p = ffidec.new('char[]', frame)
        # frame_in has to memcpy the frame out
        # because frame_p can be deallocated after leaving this method
        self.clib.frame_in(self.decoder, frame_p, len(frame))


class DummyDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decoders."""

    def __init__(self, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.clib, self.ffi = libdummy, ffidummy

    def decode(self):
        """Returns nblist
        :returns: list of tuples (double-probability, string-sentence)

        """
        prob_p, ans_p, ans_size = ffidec.new(
            'double *'), ffidec.new('char **'), ffidec.new('size_t *')
        libdec.return_answer(prob_p, ans_p, ans_size)
        ans, prob = self.ffi.string(ans_p[0], ans_size[0]), prob_p[0]
        return [(prob, ans)]


class OnlineDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decoders."""

    def __init__(self, argv, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.argv = argv
        self.decoder = self.__enter__()

    def __enter__(self):
        dec_pointer = ffidec.new("OnlineDecoderPointer")
        argc, argv = len(self.argv), self.argv
        # allocate the C decoder
        dec_pointer = libdec.new_KaldiDecoderWrapper(argc, argv)
        return dec_pointer

    def __exit__(self, exc_type, exc_val, exc_tb):
        # deallocate the C decoder
        libdec.del_KaldiDecoderWrapper(self.decoder)

    def close(self):
        self.__exit__(None, None, None)

    def frame_in(self, frame_str):
        libdec.FrameIn(self.decoder, frame_str, len(frame_str))

    def decode(self):
        """Ask the decoder to process the buffered data.
        Does not return any output.
        """
        libdec.Decode(self.decoder)

    def prepare_hyp(self):
        full_hyp_p = ffidec.new("int *")
        size = libdec.PrepareHypothesis(self.decoder, full_hyp_p)
        # TODO should I convert size?
        return (size, full_hyp_p[0])

    def get_hypothesis(self, size):
        # TODO our decoder does not return any measure of quality for the decoded hypothesis
        # prob_p = ffidec.new('double *')
        hyp = numpy.zeros(size).astype('int32')
        hyp_p = ffidec.cast("int *", hyp.ctypes.data)
        libdec.GetHypothesis(self.decoder, hyp_p, size)
        prob = 1.0  # TODO prob = prob_p[0]  # TODO
        return (prob, hyp)


class ConfNetDecoder(KaldiDecoder):
    """Docstring for ConfNetDecoder """

    def __init__(self):
        """@todo: to be defined """
        pass
