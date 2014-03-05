from __future__ import unicode_literals


class PyKaldiError(Exception):
    pass


class PyKaldiCError(PyKaldiError):
    def __init__(self, retcode):
        self.retcode = retcode

    def __str__(self):
        ver = super(PyKaldiError, self).__str__()
        return '%s\nFailed with return code: %s' % repr(ver, self.retcode)


class PyKaldiInstallError(PyKaldiCError):
    pass
