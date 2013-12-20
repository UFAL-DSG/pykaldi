all:

include ../kaldi.mk

# FIXME TODO TEST with shared settings

# Tag the compilation with version
EXTRA_CXXFLAGS += -DPYKALDI_MAJOR=0 -DPYKALDI_MINOR=0 -DPYKALDI_MINOR=0 -DPYKALDI_PATCH=1
# Git version is also stored to library
GIT_HASH=$(shell git log -1 HEAD --pretty=format:"%H")
EXTRA_CXXFLAGS += -DPYKALDI_GIT_VERSION=\"$(GIT_HASH)\"
EXTRA_CXXFLAGS += -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../decoder/kaldi-decoder.a ../thread/kaldi-thread.a \
		../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a

BINFILES =

TESTFILES = pykaldi-faster-wrapper-test pykaldi-feat-input-test \
			pykaldi-audio-source-test read-wav-test pykaldi-utils-test

OBJFILES = pykaldi-latgen-decoder.o pykaldi-latgen-wrapper.o \
		   pykaldi-faster-wrapper.o pykaldi-faster-decoder.o \
		   pykaldi-audio-source.o  pykaldi-utils.o \
		   pykaldi-decodable.o pykaldi-feat-input.o

LIBNAME = dec-wrap


include ../makefiles/default_rules.mk
