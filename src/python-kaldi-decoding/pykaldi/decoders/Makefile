# PYTHON C api test settings
PY_LIBNAME = dummyio
PY_LIBFILE = lib$(PY_LIBNAME).so
PY_OBJFILES = dummyio.o

# PYTHON C api test
kaldi_decoders.py: $(PY_LIBFILE)

kaldi_decoders_test.py:  kaldi_decoders.py
	python $@  # using unit test
