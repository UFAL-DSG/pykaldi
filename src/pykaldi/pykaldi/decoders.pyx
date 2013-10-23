# encoding: utf-8
# distutils: language = c++
from libc.stdlib cimport malloc, free
from libcpp.vector cimport vector
from libcpp cimport bool


# include "dec-wrap/pykaldi-faster-wrapper.h"
# include "dec-wrap/pykaldibin-util.h"
cdef extern from "dec-wrap/pykaldi-latgen-wrapper.h" namespace "kaldi":
    cdef cppclass GmmLatgenWrapper:
        size_t Decode(size_t max_frames)
        void FrameIn(unsigned char *frame, size_t frame_len)
        int GetBestPath(vector[int] v_out)
        void PruneFinal()
        void Reset(bool keep_buffer_data)
        int Setup(int argc, char **argv) except +

cdef class PyGmmLatgenWrapper:
    cdef GmmLatgenWrapper * thisptr

    def __cinit__(self):
        self.thisptr = new GmmLatgenWrapper()

    def decode(self, max_frames):
        return self.thisptr.Decode(max_frames)

    def frame_in(self, bytes frame_str, int num_samples):
        assert len(frame_str) == (2 * num_samples), "Check for 16bit audio" # TODO REMOVE
        self.thisptr.FrameIn(frame_str, num_samples)
    
    def get_best_path(self):
        cdef vector[int] t
        self.thisptr.GetBestPath(t)
        return [t[i] for i in xrange(t.size())]

    def get_Nbest(self):
        pass

    def prune_final(self):
        self.thisptr.PruneFinal()

    def reset(self, keep_buffer_data):
        self.thisptr.Reset(keep_buffer_data)

    #def setup(self, args): 
    #    cdef char **string_buf = malloc(len(args) * sizeof(char*))
    #    for i in range(len(args)):
    #        string_buf[i] = PyString_AsString(args[i])
    #    self.thisptr.Setup(len(args), string_buf)
    #    free(string_buf)
