all:

include ../src/kaldi.mk

EXTRA_CXXFLAGS += -I.. -I../src -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../src/online2/kaldi-online2.a ../src/decoder/kaldi-decoder.a \
		../src/lat/kaldi-lat.a ../src/hmm/kaldi-hmm.a ../src/transform/kaldi-transform.a ../src/gmm/kaldi-gmm.a \
		../src/fstext/kaldi-fstext.a ../src/tree/kaldi-tree.a \
		../src/feat/kaldi-feat.a ../src/matrix/kaldi-matrix.a ../src/util/kaldi-util.a ../src/base/kaldi-base.a

BINFILES = onl-rec-latgen-recogniser-demo

TESTFILES =  onl-rec-utils-test

OBJFILES = onl-rec-latgen-recogniser.o onl-rec-utils.o

LIBNAME = onl-rec


include ../src/makefiles/default_rules.mk

cleantest:
	rm -f *.txt *.svg *.fst

cleanall: clean cleantest
