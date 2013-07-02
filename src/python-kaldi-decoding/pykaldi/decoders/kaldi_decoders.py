from cffi import FFI
import os

header = '''
typedef unsigned long mysizet;
void return_answer(double * prob, char **ans, mysizet *size);
void frame_in(char *str_frame, mysizet size);
'''
ffi = FFI()
ffi.cdef(header)


def add_ld_library_path(newdir):
    lib_path = os.environ.get('LD_LIBRARY_PATH', '')
    if not lib_path:
        os.environ['LD_LIBRARY_PATH'] = newdir
    else:
        os.environ['LD_LIBRARY_PATH'] = newdir + os.pathsep + lib_path


class KaldiDecoder(object):
    """Python wrapper for accessing C Kaldi decoders.
    """

    def __init__(self, shared_lib, fs=16000, nchan=1):
        """@todo: Docstring for __init__
        :fs: Sampling frequency
        :nchan: Number of channels
        """
        # Loading shared library
        if os.path.isabs(shared_lib):
            self.shared_lib_dir = os.path.dirname(shared_lib)
            self.shared_lib_path = shared_lib
        else:
            # If specified just by name -> look at the same directory of this module
            self.shared_lib_dir = os.path.dirname(os.path.realpath(__file__))
            self.shared_lib_path = os.path.join(self.shared_lib_dir, shared_lib)
        add_ld_library_path(self.shared_lib_dir)
        self.cdecoder = ffi.dlopen(self.shared_lib_path)

        # FIXME send the settings to the Recorder maybe verify them
        self.fs = fs
        self.nchan = nchan

    def rec_in(self, frame):
        """Recieving audio frame by frame, so defining asynchronous API.
        The input API is the same for all decoders.
        :frame: @todo
        :returns: @todo
        """
        frame_p = ffi.new('char[]', frame)
        # frame_in has to memcpy the frame out
        # because frame_p can be deallocated after leaving this method
        self.cdecoder.frame_in(frame_p, len(frame))


class NbListDecoder(KaldiDecoder):
    """NbListDecoder returns nblist
    it has the same input as other decoders."""

    def __init__(self, **kwargs):
        """Adds specific cdecoder - for nblists"""
        shared_lib = "libdummyio.so"  # Shared lib expected in this directory
        KaldiDecoder.__init__(self, shared_lib, **kwargs)

    def decode(self):
        """Returns nblist
        :returns: list of tuples (double-probability, string-sentence)

        """
        prob_p, ans_p, ans_size = ffi.new('double *'), ffi.new('char **'), ffi.new('mysizet *')
        self.cdecoder.return_answer(prob_p, ans_p, ans_size)
        ans, prob = ffi.string(ans_p[0], ans_size[0]), prob_p[0]
        return [(prob, ans)]


class ConfNetDecoder(KaldiDecoder):
    """Docstring for ConfNetDecoder """

    def __init__(self):
        """@todo: to be defined """
        pass
