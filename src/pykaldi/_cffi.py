from cffi import FFI
from exceptions import
ffi = FFI()


def pykaldi_version():
    raise NotImplementedError('FIXME below is dummy implementation')
    ffi_check = FFI()
    ffi_check.cdef('void fst_version(int *major, int *minor, int *patch);')
    C_check_version = ffi_check.verify('#include <fst/fst.h>', libraries=['fst'])
    major = ffi.new('int*')
    minor = ffi.new('int*')
    patch = ffi.new('int*')

    C_check_version.fst_version(major, minor, patch)

    return (int(major[0]), int(minor[0]), int(patch[0]))
