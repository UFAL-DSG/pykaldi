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


from pykaldi import ffidec, libdec
from pykaldi.exceptions import PyKaldiError


class KaldiDecoder(object):
    """Python wrapper for accessing C Kaldi decs.
    """

    def __init__(self, fs=16000, nchan=1):
        """@todo: Docstring for __init__
        :fs: Sampling frequency
        :nchan: Number of channels
        """
        assert fs == 16000, 'Only 16000 audio is supported'
        assert nchan == 1, 'Only 1 channel is supported'
        self.fs, self.nchan = fs, nchan
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


class DecoderCloser:
    '''A context manager for KaldiDecoder'''

    def __init__(self, dec):
        if dec is KaldiDecoder:
            raise PyKaldiError("Not a decoder")
        self.dec = dec

    def __enter__(self):
        return self.dec

    def __exit__(self, exception_type, exception_val, trace):
        self.dec.close()


class OnlineDecoder(KaldiDecoder):
    """Online API means that the best hypothesis (or its part) can
    be returned almost instantly also in the middle of the utterance."""

    def __init__(self, args, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.lib, self.ffi = libdec, ffidec
        # first argument is the name of the "program" like in C
        argv = ['OnlineDecoder'] + args
        # necessary to keep it alive long enough -> member field-> ok
        self.argv = [self.ffi.new("char[]", arg) for arg in argv]
        argc, argp = len(self.argv), self.ffi.new("char *[]", self.argv)
        self.dec = self.lib.new_KaldiDecoderWrapper()
        if self.lib.Setup(self.dec, argc, argp) != 0:
            # FIXME use custom exception class eg PykaldiOnlineDecoderArgsError
            raise Exception("OnlineDecoder started with wrong parameters!")

    def decode(self):
        """ Ask the decoder to process the buffered audio data.
        The decoder will return a part of hypothesis, if safe.
        Returns: list. Possibly very short (or empty) list of word_ids.
        """
        size = self.lib.Decode(self.dec)
        return self._pop_hyp_from_c(size)

    def finished(self):
        """ Returns: bool. Indicating if decoder processed all features
        and does not wait for more features."""
        return self.lib.Finished()

    def finish_decoding(self):
        """Tell the decoder that no more input is coming
        and to decode last hypothesis.
        Returns: int. The size of decoded hypotheses."""
        size = self.lib.FinishDecoding(self.dec)
        return self._pop_hyp_from_c(size)

    def frame_in(self, frame_str, num_samples):
        assert len(frame_str) == (2 * num_samples), "We support only 16bit audio"
        "-> 1 sample == 2 chars -> len(frame_str) = 2 * num_samples"
        self.lib.FrameIn(self.dec, frame_str, num_samples)

    def close(self):
        """Deallocates the underlaying C module.
        Do not use the object after calling close!"""
        self._deallocate()

    def _deallocate(self):
        if self.dec is not None:
            self.lib.del_KaldiDecoderWrapper(self.dec)
            self.dec = None

    def __del__(self):
        self._deallocate()

    def _pop_hyp_from_c(self, size):
        # TODO our dec does not return any measure of quality for the decoded hypothesis
        # prob_p = self.ffi.new('double *')
        hyp_p = self.ffi.new("int []", size)
        self.lib.PopHyp(self.dec, hyp_p, size)
        hyp = []
        for i in xrange(size):
            hyp.append(hyp_p[i])
        prob = 1.0  # TODO get real prob from C in feature and dereference it: prob = prob_p[0]
        return (hyp, prob)


class OnlineDecoderNumpy(OnlineDecoder):
    '''Inherits all interface from OnlineDecoder and overrides
    the get_hypothesis method to return Numpy array'''

    def __init__(self, args, **kwargs):
        try:
            import numpy
        except:
            # FIXME use custom exception class eg PykaldiOnlineDecoderArgsError
            raise Exception("Install numpy for OnlineDecoderNumpy!")
        OnlineDecoder.__init__(self, args, **kwargs)

    def get_hypothesis(self, size):
        # TODO our dec does not return any measure of quality for the decoded hypothesis
        # prob_p = self.ffi.new('double *')
        hyp = numpy.zeros(size).astype('int32')
        hyp_p = self.ffi.cast("int *", hyp.ctypes.data)
        self.lib.GetHypothesis(self.dec, hyp_p, size)
        prob = 1.0  # TODO get real prob from C in feature and dereference it: prob = prob_p[0]
        return (prob, hyp)


class ConfNetDecoder(KaldiDecoder):
    """Docstring for ConfNetDecoder """

    def __init__(self):
        """@todo: to be defined """
        pass


class DummyDecoder(object):
    """For debugging purposes."""

    def __init__(self, *args, **kwargs):
        print 'arg:\n%s\nkwargs:%s\n' % str(*args, **kwargs)

    def rec_in(self, frame):
        print 'Dummy enqueing frame of length %d' % len(frame)

    def decode(self):
        return [(1.0, 'My first dummy answer')]
