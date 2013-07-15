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

TESTFILES = compute-cmvn-stats-test compute-wer-test gmm-latgen-faster-test compute-mfcc-feats-test lattice-best-path-test online-wav-gmm-decode-faster-test online-python-gmm-decode-faster-test

OBJFILES = compute-wer.o gmm-latgen-faster.o compute-mfcc-feats.o lattice-best-path.o online-python-gmm-decode-faster.o online-wav-gmm-decode-faster.o compute-cmvn-stats.o 

LIBNAME = pykaldi

ADDLIBS = ../decoder/kaldi-decoder.a ../online/kaldi-online.a ../thread/kaldi-thread.a \
		  ../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		  ../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		  ../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a


include ../makefiles/default_rules.mk
