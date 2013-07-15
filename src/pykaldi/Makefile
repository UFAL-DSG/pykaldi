all:

EXTRA_CXXFLAGS = -Wno-sign-compare -I ../../tools/portaudio/install/include # -pedantic
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging
EXTRA_LDLIBS =

include ../kaldi.mk

UNAME=$(shell uname)
ifeq ($(UNAME), Linux)
ifeq ($(KALDI_FLAVOR), dynamic)
		#The PA_RingBuffer interface is internal and is not exported in the so libray
		#so we have to compile in the static one
		#.PA_ABS_PATH := $(shell readlink -f ../../tools/portaudio/install/lib )
		#EXTRA_LDLIBS = -L ../../tools/portaudio/install/lib/ -Wl,-rpath=$(.PA_ABS_PATH) -lportaudio
    EXTRA_LDLIBS = ../../tools/portaudio/install/lib/libportaudio.a
else
    EXTRA_LDLIBS = ../../tools/portaudio/install/lib/libportaudio.a
endif
ifneq ($(wildcard ../../tools/portaudio/install/include/pa_linux_alsa.h),)
    EXTRA_LDLIBS += -lasound
else
    EXTRA_LDLIBS += -lrt
endif
else
    EXTRA_LDLIBS = -L ../../tools/portaudio/install/lib/ -lportaudio
endif

BINFILES =

TESTFILES = compute-wer-test online-python-gmm-decode-faster-test

OBJFILES = compute-wer.o online-python-gmm-decode-faster.o

LIBNAME = kaldi-python-kaldi-decoding

PYTHON_LIB = libpykaldi.so


ADDLIBS = ../decoder/kaldi-decoder.a ../online/kaldi-online.a ../thread/kaldi-thread.a \
		  ../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		  ../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		  ../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a

# FIXME LINUX ONLY
$(PYTHON_LIB): $(OBJFILES) $(ADDLIBS) lib$(LIBNAME).so
ifneq ($(KALDI_FLAVOR), dynamic)
	$(warning The following command will probably fail, in that run configure with --shared flag and remake all.)
endif
	# Building shared library from static one. The static one was compiled with -fPIC.
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(EXTRA_LDLIBS) $(LDFLAGS) $(LDLIBS)


include ../makefiles/default_rules.mk
