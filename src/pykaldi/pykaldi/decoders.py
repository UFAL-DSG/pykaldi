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


class DecoderCloser:
    '''A context manager for decoders'''

    def __init__(self, dec):
        self.dec = dec

    def __enter__(self):
        return self.dec

    def __exit__(self, exception_type, exception_val, trace):
        self.dec.close()


class PykaldiFasterDecoder(KaldiDecoder):
    """Online API means that the best hypothesis (or its part) can
    be returned almost instantly also in the middle of the utterance."""

    def __init__(self, args, **kwargs):
        KaldiDecoder.__init__(self, **kwargs)
        self.lib, self.ffi = libdec, ffidec
        # first argument is the name of the "program" like in C
        argv = ['PykaldiFasterDecoder'] + args
        # necessary to keep it alive long enough -> member field-> ok
        self.argv = [self.ffi.new("char[]", arg) for arg in argv]
        argc, argp = len(self.argv), self.ffi.new("char *[]", self.argv)
        self.dec = self.lib.new_KaldiDecoderWrapper()
        if self.lib.Setup(self.dec, argc, argp) != 0:
            raise Exception("PykaldiFasterDecoder started with wrong parameters!")

    def decode(self, force_end_utt=False):
        """ Ask the decoder to process the buffered audio data.
        The decoder will return a part of hypothesis, if safe.
        Returns: list. Possibly very short (or empty) list of word_ids.
        """
        size = self.lib.Decode(self.dec, force_end_utt)
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


class PykaldiLatgenFasterDecoder(KaldiDecoder):
    """Decoding lattices"""

    def __init__(self, args, **kwargs):
        self.lib, self.ffi = libdec, ffidec
        # first argument is the name of the "program" like in C
        argv = ['PykaldiLatgenFasterDecoder'] + args
        # necessary to keep it alive long enough -> member field-> ok
        self.wrapper_p = self.lib.new_GmmLatgenWrapper()
        self.argv = [self.ffi.new("char[]", arg) for arg in argv]
        argc, argp = len(self.argv), self.ffi.new("char *[]", self.argv)
        if self.lib.GmmLatgenWrapper_Setup(argc, argp, self.wrapper_p) != 0:
            raise Exception("PykaldiLatgenFasterDecoder started with wrong parameters!")
        self.lib.GmmLatgenWrapper_Reset(self.wrapper_p, False)

    def decode(self, max_frames=1):
        """The decoder will process at maximum max_frames in forward decoding.
        Returns the number of actually processed frames"""
        w = self.wrapper_p
        return self.lib.GmmLatgenWrapper_Decode(w.decoder, w.decodable, max_frames)

    def frame_in(self, frame_str, num_samples):
        assert len(frame_str) == (2 * num_samples), "We support only 16bit audio"
        "-> 1 sample == 2 chars -> len(frame_str) = 2 * num_samples"
        self.lib.GmmLatgenWrapper_FrameIn(self.wrapper_p.audio, frame_str, num_samples)

    def prune_final(self):
        self.lib.GmmLatgenWrapper_PruneFinal(self.wrapper_p.decoder)

    def get_best_path(self):
        # FIXME extract fst_p from Python object
        void_fst_p = self.lib.new_lat_fst()
        self.lib.GmmLatgenWrapper_GetBestPath(self.wrapper_p.decoder, void_fst_p)
        self.lib.print_linear_fst(void_fst_p)
        self.lib.del_lat_fst(void_fst_p)
        self.lib.GmmLatgenWrapper_Reset(self.wrapper_p, False)

    def close(self):
        """Deallocates the underlaying C module.
        Do not use the object after calling close!"""
        self._deallocate()

    def _deallocate(self):
        if self.wrapper_p is not None:
            self.lib.del_GmmLatgenWrapper(self.wrapper_p)
            self.wrapper_p = None

    def __del__(self):
        self._deallocate()


class DummyDecoder(object):
    """For debugging purposes."""

    def __init__(self, *args, **kwargs):
        print 'arg:\n%s\nkwargs:%s\n' % str(*args, **kwargs)

    def rec_in(self, frame):
        print 'Dummy enqueing frame of length %d' % len(frame)

    def decode(self):
        return [(1.0, 'My first dummy answer')]
