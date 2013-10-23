# encoding: utf-8
# distutils: language = c++
from libc.stdlib cimport malloc, free
from libcpp.vector cimport vector
from libcpp cimport bool


# include "dec-wrap/pykaldi-faster-wrapper.h"
# include "dec-wrap/pykaldi-utils.h"
cdef extern from "dec-wrap/pykaldi-latgen-wrapper.h" namespace "kaldi":
    cdef cppclass GmmLatgenWrapper:
        size_t Decode(size_t max_frames)
        void FrameIn(unsigned char *frame, size_t frame_len)
        int GetBestPath(vector[int] v_out)
        int GetNbest(n, vector[vector[int]] v_out)
        #int GetRawLattice(Lattice lat_out)
        #int GetLattice(CompactLattice clat_out)
        void PruneFinal()
        void Reset(bool keep_buffer_data)
        int Setup(int argc, char **argv) except +


class DecoderCloser:
    '''A context manager for decoders'''

    def __init__(self, dec):
        self.dec = dec

    def __enter__(self):
        return self.dec

    def __exit__(self, exception_type, exception_val, trace):
        self.dec.close()


cdef class PyGmmLatgenWrapper:
    cdef GmmLatgenWrapper * thisptr

    def __cinit__(self):
        self.thisptr = new GmmLatgenWrapper()

    def __init__(self, fs=16000, nchan=1, bits=16):
        self.fs, self.nchan, self.bits = fs, nchan, bits
        assert(self.bits % 8 == 0)

    def __dealloc__(self):
        del self.thisptr

    def decode(self, max_frames):
        return self.thisptr.Decode(max_frames)

    def frame_in(self, bytes frame_str, int num_samples):
        assert len(frame_str) == ((self.bits / 8) * num_samples), "Length of audio and bits mismatch"
        self.thisptr.FrameIn(frame_str, num_samples)

    def get_best_path(self):
        cdef vector[int] t
        self.thisptr.GetBestPath(t)
        return [t[i] for i in xrange(t.size())]

    def get_Nbest(self):
        pass

    def get_lattice(self):
        pass

    def get_raw_lattice(self):
        pass

    def prune_final(self):
        self.thisptr.PruneFinal()

    def reset(self, keep_buffer_data):
        self.thisptr.Reset(keep_buffer_data)

    def setup(self, args):
        args = ['PyGmmLatgenWrapper'] + args
        cdef char **string_buf = <char**>malloc(len(args) * sizeof(char*))
        if string_buf is NULL:
            raise MemoryError()
        try:
            for i, s in enumerate(args):
                string_buf[i] = s
            self.thisptr.Setup(len(args), string_buf)
        finally:
            free(string_buf)


class DummyDecoder(object):
    """For debugging purposes."""

    def __init__(self, *args, **kwargs):
        print 'arg:\n%s\nkwargs:%s\n' % str(*args, **kwargs)

    def rec_in(self, frame):
        print 'Dummy enqueing frame of length %d' % len(frame)

    def decode(self):
        return [(1.0, 'My first dummy answer')]
