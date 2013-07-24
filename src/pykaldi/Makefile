all:

include ../kaldi.mk
ifneq ($(KALDI_FLAVOR), dynamic)
$(error Kaldi must compiled with dynamic libraries support. Run configure with --shared flag. )
endif
EXTRA_CXXFLAGS = -Wno-sign-compare -I ../../tools/portaudio/install/include
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging

# Kaldi libraries required by the pykaldi
ADDLIBS = ../decoder/kaldi-decoder.a ../online/kaldi-online.a ../thread/kaldi-thread.a \
		../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a

EXTRA_LDLIBS += ../../tools/portaudio/install/lib/libportaudio.a
ifneq ($(wildcard ../../tools/portaudio/install/include/pa_linux_alsa.h),)
    EXTRA_LDLIBS += -lasound
else
    EXTRA_LDLIBS += -lrt
endif

BINFILES = 

TESTFILES = compute-wer-test pykaldi-gmm-decode-faster-test

OBJFILES = compute-wer.o pykaldi-gmm-decode-faster.o pykaldi-audio-source.o \
		   pykaldi-decodable.o pykaldi-feat-input.o \
		   pykaldi-faster-decoder.o

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



decoders.py: $(LIBFILE)

decoders_test.py:  decoders.py

# extracts testing data
DECODE_DIR = work
include online-data.mk

pytest: decoders_test.py $(DATA_TAR)
	nosetests -s decoders_test.py  # -s let the output be printed



clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) $(TESTFILES) *.pyc *.o *.so 

depend:
	-$(CXX) -M $(CXXFLAGS) *.cc > .depend.mk  

-include .depend.mk

