"""
Module with Exceptioin for pykaldi package
"""
from __future__ import unicode_literals
from kaldi import __version__, __git_revision__


class PyKaldiError(Exception):

    """Geneneral Pykaldi exception"""

    def __str__(self):
        return 'Pykaldi %s, Git revision %s' % (__version__, __git_revision__)


class PyKaldiRuntimeError(PyKaldiError):

    """Runtime Kaldi exception"""

    def __init__(self, retcode):
        self.retcode = retcode

    def __str__(self):
        ver = super(PyKaldiError, self).__str__()
        return '%s\nFailed with return code: %s' % repr(ver, self.retcode)


class PyKaldiInstallError(PyKaldiError):

    """Exception raised during installation or setup"""

    pass
