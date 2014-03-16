all:

include ../kaldi.mk

EXTRA_CXXFLAGS += -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../decoder/kaldi-decoder.a ../thread/kaldi-thread.a \
		../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a

BINFILES =

TESTFILES = onl-rec-feat-input-test \
			onl-rec-latgen-recogniser-test \
			onl-rec-audio-source-test read-wav-test onl-rec-utils-test

OBJFILES = onl-rec-latgen-decoder.o onl-rec-latgen-recogniser.o \
		   onl-rec-audio-source.o  onl-rec-utils.o \
		   onl-rec-decodable.o onl-rec-feat-input.o

LIBNAME = onl-rec


include ../makefiles/default_rules.mk

cleantest:
	rm -f *.txt *.svg *.fst

cleanall: clean cleantest
