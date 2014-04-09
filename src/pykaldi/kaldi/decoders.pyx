# encoding: utf-8
# distutils: language = c++
from __future__ import unicode_literals

from cython cimport address
from libc.stdlib cimport malloc, free
from libcpp.vector cimport vector
from libcpp cimport bool
cimport fst._fst
cimport fst.libfst
import fst
from kaldi.utils import lattice_to_nbest


cdef extern from "onl-rec/onl-rec-latgen-recogniser.h" namespace "kaldi":
    cdef cppclass OnlineLatgenRecogniser:
        size_t Decode(size_t max_frames) except +
        void FrameIn(unsigned char *frame, size_t frame_len) except +
        bool GetBestPath(vector[int] v_out, float *lik) except +
        bool GetRawLattice(fst.libfst.StdVectorFst *fst_out) except +
        bool GetLattice(fst.libfst.LogVectorFst *fst_out, double *tot_lik) except +
        void PruneFinal() except +
        void Reset(bool keep_buffer_data) except +
        int Setup(int argc, char **argv) except +


cdef class PyOnlineLatgenRecogniser:

    """PyOnlineLatgenRecogniser

    Python wrapper around C++ Kaldi OnlineLatgenRecogniser
    which provides on-line speech recognition interface
    """

    cdef OnlineLatgenRecogniser * thisptr
    cdef long fs
    cdef int nchan, bits

    def __cinit__(self):
        self.thisptr = new OnlineLatgenRecogniser()

    def __init__(self, fs=16000, nchan=1, bits=16):
        """ __init__(self, fs=16000, nchan=1, bits=16)
        
        Initialise recognizer with audio input stream parameters.
        """
        self.fs, self.nchan, self.bits = fs, nchan, bits
        assert(self.bits % 8 == 0)

    def __dealloc__(self):
        del self.thisptr

    def decode(self, max_frames=10):
        """decode(self, max_frames)

        Decodes at maximum max_frames.
        Return number of actually decoded frames in range [0, max_frames].
        The decoding has RTF < 1.0 on common computer.
        Consequently, for frames shift 10 ms the decoding
        should be faster than 0.01 * max_frames seconds."""
        return self.thisptr.Decode(max_frames)

    def frame_in(self, bytes frame_str):
        """frame_in(self, bytes frame_str, int num_samples)

        Accepts input raw audio data and interpret them
        according sample width (self.bits) settings"""
        num_bytes = (self.bits / 8)
        num_samples = len(frame_str) / num_bytes
        assert(num_samples * num_bytes == len(frame_str)), "Not align audio to for %d bits" % self.bits
        self.thisptr.FrameIn(frame_str, num_samples)

    def get_best_path(self):
        """get_best_path(self)

        Returns one-best ASR hypothesis
        directly from internal representation."""
        cdef vector[int] t
        cdef float lik
        self.thisptr.GetBestPath(t, address(lik))
        ids = [t[i] for i in xrange(t.size())]
        return (lik, ids)

    def get_nbest(self, n=1):
        """get_nbest(self, n=1)

        Returns n-best list extracted from word posterior lattice."""
        lik, lat = self.get_lattice()
        return lattice_to_nbest(lat, n)

    def get_lattice(self):
        """get_lattice(self)

        Return word posterior lattice and its likelihood.
        It may last non-trivial amount of time e.g. 100 ms."""
        cdef double lik
        r = fst.LogVectorFst()
        self.thisptr.GetLattice((<fst._fst.LogVectorFst?>r).fst, address(lik))
        return (lik, r)

    def get_raw_lattice(self):
        """get_raw_lattice(self)

        Return state level lattice.
        It may last non-trivial amount of time e.g. 100 ms."""
        r = fst.StdVectorFst()
        self.thisptr.GetRawLattice((<fst._fst.StdVectorFst?>r).fst)
        return r

    def prune_final(self):
        """prune_final(self)

        It prepares internal representation for lattice extration."""
        self.thisptr.PruneFinal()

    def reset(self, keep_buffer_data=True):
        """reset(self, keep_buffer_data)

        Resets the frame counter and prepare decoder for new utterance.
        Dependently on keep_buffer_data parameter clear the buffered data.
        If the (audio) data are kept they are the first
        input data for new utterance."""
        self.thisptr.Reset(keep_buffer_data)

    def setup(self, args):
        """setup(self, args)

        Accepts string arguments for setting speech parameterisation, feature transformation
        and decoder component.
        args is list of string of command line arguments which are passed to C++ Kaldi parser.
        If wrong arguments are specified the usage is printed.
        """
        args = ['PyOnlineLatgenRecogniser'] + args
        cdef char **string_buf = <char**>malloc(len(args) * sizeof(char*))
        if string_buf is NULL:
            raise MemoryError()
        try:
            for i, s in enumerate(args):
                string_buf[i] = s
            self.thisptr.Setup(len(args), string_buf)
        finally:
            free(string_buf)
