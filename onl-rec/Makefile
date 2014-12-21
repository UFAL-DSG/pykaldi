all:

include ../kaldi/src/kaldi.mk

EXTRA_CXXFLAGS += -I.. -I../kaldi/src -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../kaldi/src/online2/kaldi-online2.a ../kaldi/src/decoder/kaldi-decoder.a \
		../kaldi/src/lat/kaldi-lat.a ../kaldi/src/hmm/kaldi-hmm.a ../kaldi/src/transform/kaldi-transform.a ../kaldi/src/gmm/kaldi-gmm.a \
		../kaldi/src/fstext/kaldi-fstext.a ../kaldi/src/tree/kaldi-tree.a \
		../kaldi/src/feat/kaldi-feat.a ../kaldi/src/matrix/kaldi-matrix.a ../kaldi/src/util/kaldi-util.a ../kaldi/src/base/kaldi-base.a

BINFILES = onl-rec-latgen-recogniser-demo

TESTFILES =  onl-rec-utils-test

OBJFILES = onl-rec-latgen-recogniser.o onl-rec-utils.o

LIBNAME = onl-rec


include ../kaldi/src/makefiles/default_rules.mk

cleantest:
	rm -f *.txt *.svg *.fst

cleanall: clean cleantest
