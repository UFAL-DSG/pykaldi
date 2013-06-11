TESTFILES = compute-wer-test gmm-latgen-faster-test compute-mfcc-feats-test lattice-best-path-test online-wav-gmm-decode-faster-test
OBJFILES = compute-wer.o gmm-latgen-faster.o compute-mfcc-feats.o lattice-best-path.o online-wav-gmm-decode-faster.o
LDLIBFILE = libcffi-kaldi.so
LIBFILE = cffi-kaldi.a
EXTRA_CXXFLAGS = -Wno-sign-compare -fPIC -I ../../tools/portaudio/install/include

include ../kaldi.mk
CC = gcc

# Check kaldi.mk for LDLIBS -> there are static libraries
# We need dynamic libraries
# We suppose that this Makefile sits in kaldi-trunk/src/ThisDirectory
OBLAS_LDD = $(PWD)/../../tools/OpenBLAS/
FST_LDD = $(PWD)/../../tools/openfst-1.3.2/lib
PA_LDD = $(PWD)/../../tools/portaudio/install/lib
LDLIBS = -L$(FST_LDD) -lfst -ldl -L$(OBLAS_LDD) -lopenblas -lm -lpthread 

UNAME=$(shell uname)
ifeq ($(UNAME), Linux)
    LDLIBS += ../../tools/portaudio/install/lib/libportaudio.a
ifneq ($(wildcard ../../tools/portaudio/install/include/pa_linux_alsa.h),)
    LDLIBS += -lasound
else
    LDLIBS += -lrt
endif
else
    LDLIBS += -L $(PA_LDD) -lportaudio
endif

all:  $(LIBFILE) $(LDLIBFILE)

### Building the shared library
# $(LDLIBFILE): $(OBJFILES) 
# 	$(CXX) -fPIC -shared $(OBJFILES) -o $@

# still need to compile *.o with fPIC
# $(LDLIBFILE): $(LIBFILE) 
# 	$(CXX) -g -O0 -fPIC -rdynamic -shared -o $@ -Wl,--whole-archive $(LIBFILE) -Wl,--no-whole-archive $(LDLIBS)


$(LDLIBFILE): $(OBJFILES) ../base/*.o ../util/*.o ../feat/*.o ../matrix/*.o ../tree/*.o \
		../fstext/*.o ../gmm/*.o ../transform/*.o  ../hmm/*.o ../lat/*.o ../thread/*.o \
		../decoder/decodable-am-diag-gmm.o ../decoder/lattice-faster-decoder.o ../decoder/faster-decoder.o \
		../online/*.o
	$(CXX) -fPIC -rdynamic -shared -Wl,-export-dynamic -o $@ $^ $(LDLIBS)

$(LIBFILE): $(OBJFILES)
	$(AR) -cru $(LIBFILE) $(OBJFILES)
	$(RANLIB) $(LIBFILE)

# Rule below would expand to, e.g.:
# ../base/kaldi-base.a:
# 	make -c ../base kaldi-base.a
# -c option to make is same as changing directory.
%.a:
	EXTRA_CXXFLAGS=-fPIC $(MAKE) -C ${@D} ${@F}

clean:
	-rm -f *.o *.a tmp* *.tmp *.so .depend.mk $(TESTFILES)

depend:
	-$(CXX) -M $(CXXFLAGS) *.cc > .depend.mk  
	$(CXX) -MD $(CXXFLAGS) *.cc $(LDLIBS) > depend-test.mk  

# removing automatic making of "depend" as it's quite slow.
# .depend.mk: depend

-include .depend.mk

.valgrind: $(TESTFILES)

.PHONY: test_dyn python_test test

### TESTS - launching command --help ### 
test_dyn: $(LDLIBFILE) $(TESTFILES)
	# Before running the binaries set the LD_LIBRARY_PATH variable as we do!
	@result=0; for x in $(TESTFILES); do \
		echo -n "Running $$x :  "; \
		LD_LIBRARY_PATH=$(OBLAS_LDD):$(FST_LDD):$:`pwd` ./$$x --help > /dev/null 2>&1 ; \
		if [ $$? -ne 0 ]; then echo "... FAIL"; result=1; else echo "... SUCCESS";  fi;  \
		done; exit $$result

python_test: run.py $(LDLIBFILE) 
	LD_LIBRARY_PATH=$(OBLAS_LDD):$(FST_LDD):`pwd` python $<
	echo $?

test: test_dyn python_test
