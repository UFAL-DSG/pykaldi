all:

include ../kaldi.mk

EXTRA_CXXFLAGS += -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../online2/kaldi-online2.a ../decoder/kaldi-decoder.a \
		../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a \
		../feat/kaldi-feat.a ../matrix/kaldi-matrix.a ../util/kaldi-util.a ../base/kaldi-base.a

BINFILES =

TESTFILES = onl-rec-latgen-recogniser-test onl-rec-utils-test

OBJFILES = onl-rec-latgen-recogniser.o onl-rec-utils.o

LIBNAME = onl-rec


include ../makefiles/default_rules.mk

cleantest:
	rm -f *.txt *.svg *.fst

cleanall: clean cleantest
