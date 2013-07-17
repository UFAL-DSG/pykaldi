class PyKaldiError(Exception):
    pass


class PyKaldiCError(PyKaldiError):
    def __init__(self, retcode):
        self.retcode = retcode

    def __str__(self):
        return 'Failed with return code: %s' % repr(self.retcode)
