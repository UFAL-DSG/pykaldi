

all:

EXTRA_CXXFLAGS = -Wno-sign-compare -I ../../tools/portaudio/install/include
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

TESTFILES = compute-cmvn-stats-test compute-wer-test gmm-latgen-faster-test compute-mfcc-feats-test lattice-best-path-test online-wav-gmm-decode-faster-test

OBJFILES = compute-wer.o gmm-latgen-faster.o compute-mfcc-feats.o lattice-best-path.o online-wav-gmm-decode-faster.o compute-cmvn-stats.o

LIBNAME = kaldi-python-kaldi-decoding

BIGLIB = libpykaldi.so


ADDLIBS = ../decoder/kaldi-decoder.a ../online/kaldi-online.a ../thread/kaldi-thread.a \
		  ../lat/kaldi-lat.a ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		  ../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		  ../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a


ifeq ($(KALDI_FLAVOR), dynamic)
ifdef LIBNAME
LIBFILE = lib$(LIBNAME).so
LDLIBS  += -l$(LIBNAME)
endif
LDFLAGS += $(foreach dep,$(ADDLIBS), -L$(dir $(dep)) )
LDFLAGS += -L.
LDFLAGS += -Wl,-rpath=$(shell readlink -f $(KALDILIBDIR))
LDLIBS  += $(foreach dep,$(ADDLIBS), -l$(notdir $(basename $(dep))) )
XDEPENDS = $(foreach dep,$(ADDLIBS), $(dir $(dep))/lib$(notdir $(basename $(dep))).so )
else
ifdef LIBNAME
LIBFILE = $(LIBNAME).a
endif
XDEPENDS = $(ADDLIBS)
endif

all: $(LIBFILE) $(BIGLIB) $(BINFILES)

$(LIBFILE): $(OBJFILES)
	$(AR) -cru $(LIBNAME).a $(OBJFILES)
	$(RANLIB) $(LIBNAME).a
ifeq ($(KALDI_FLAVOR), dynamic)
	# Building shared library from static (static was compiled with -fPIC)
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $(LIBNAME).a -Wl,--no-whole-archive
	cp $@ $(KALDILIBDIR)
endif

# FIXME I do not need all the LDLIBS! I need just openfst, ATLAS/OpenBLAS, portaudio
# and I certainly do not need $(LIBFILE)!
$(BIGLIB): $(OBJFILES) $(ADDLIBS) $(LIBFILE)
ifneq ($(KALDI_FLAVOR), dynamic)
	$(warning The following command will probably fail, in that run configure with --shared flag and remake all.)
endif
	# Building shared library from static one. The static one was compiled with -fPIC.
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(LDFLAGS) $(LDLIBS)


$(BINFILES): $(LIBFILE) $(XDEPENDS)


# Rule below would expand to, e.g.:
# ../base/kaldi-base.a:
# 	make -c ../base kaldi-base.a
# -c option to make is same as changing directory.
%.a:
	$(MAKE) -C ${@D} ${@F}

%.so:
	$(MAKE) -C ${@D} ${@F}

clean:
		-rm -f *.o *.a tmp* *.tmp *.so $(BINFILES) $(TESTFILES) .depend.mk valgrind.out

$(TESTFILES): $(LIBFILE) $(XDEPENDS)

test_compile: $(TESTFILES)
  
test: test_compile
	@result=0; for x in $(TESTFILES); do echo -n "Running $$x ..."; ./$$x >/dev/null 2>&1; RES=$$?; if [ $$RES -ne 0 ]; then echo "... FAIL $$RES"; result=1; else echo "... SUCCESS";  fi;  done; exit $$result

.valgrind: $(BINFILES) $(TESTFILES)


depend:
	-$(CXX) -M $(CXXFLAGS) *.cc > .depend.mk  

# removing automatic making of "depend" as it's quite slow.
#.depend.mk: depend

-include .depend.mk
