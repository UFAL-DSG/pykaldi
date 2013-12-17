all:

include ../kaldi.mk
ifneq ($(KALDI_FLAVOR), dynamic)
$(error Kaldi must compiled with dynamic libraries support. Run configure with --shared flag. )
endif


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

LIBNAME = pykaldi
LIBFILE = lib$(LIBNAME).so

all: $(LIBFILE) $(PYTHON_LIB)


$(LIBFILE): $(OBJFILES) $(ADDLIBS)
	# Building shared library from static librariess.
	# The static libraries were compiled with -fPIC.
	$(CXX) -shared -DPIC -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(EXTRA_LDLIBS) $(LDFLAGS) $(LDLIBS)


$(TESTFILES): $(LIBFILE)

test_compile: $(TESTFILES)

test: test_compile
	@result=0; for x in $(TESTFILES); do printf "Running $$x ..."; ./$$x >/dev/null 2>&1; if [ $$? -ne 0 ]; then echo "... FAIL"; result=1; else echo "... SUCCESS";  fi;  done; exit $$result


clean:
	rm -rf $(TESTFILES) *.o *.so *.svg *.fst *.txt

depend:
	-$(CXX) -M $(CXXFLAGS) *.cc > .depend.mk

-include .depend.mk
