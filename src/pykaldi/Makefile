all:

LIBNAME=pykaldi
FSTDIR=$(PWD)/../../tools/openfst

include ../kaldi.mk

# Tag the compilation with version
EXTRA_CXXFLAGS += -DPYKALDI_MAJOR=0 -DPYKALDI_MINOR=0 -DPYKALDI_MINOR=0 -DPYKALDI_PATCH=1
# Git version is also stored to library
GIT_HASH=$(shell git log -1 HEAD --pretty=format:"%H")
EXTRA_CXXFLAGS += -DPYKALDI_GIT_VERSION=\"$(GIT_HASH)\"
EXTRA_CXXFLAGS += -Wno-sign-compare
EXTRA_CXXFLAGS += -ggdb -O0  # for gdb debugging, example -DDEBUG_POST for MovePostToArcs debug

# Kaldi libraries required
ADDLIBS = ../onl-rec/onl-rec.a ../decoder/kaldi-decoder.a \
		  ../thread/kaldi-thread.a ../lat/kaldi-lat.a \
		  ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a



# ifeq ($(KALDI_FLAVOR), dynamic)
# dynamic compilation
LIBFILE=kaldi/lib$(LIBNAME).so
$(LIBFILE): $(ADDLIBS)
	# Building shared library from static librariess.
	# The static libraries were compiled with -fPIC.
	$(CXX) -shared -DPIC -o $@ -Wl,-soname=lib$(LIBNAME).so,--whole-archive $^ -Wl,--no-whole-archive $(EXTRA_LDLIBS) $(LDFLAGS) $(LDLIBS)
# else
# # TODO
# # static compilation
# LIBFILE=$(LIBNAME).a
# endif

pyfst:
	git clone https://github.com/UFAL-DSG/pyfst.git pyfst

# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so: pyfst
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include python setup.py build_ext --inplace

pyfst_compiled: pyfst/fst/_fst.so

kaldi/decoders.so: $(LIBFILE) pyfst_compiled
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py build_ext --inplace

all: kaldi/decoders.so

test: all
	LD_LIBRARY_PATH=./kaldi:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
	python setup.py nosetests

clean:
	@# removing build directory
	-python setup.py clean --all
	rm -f kaldi/decoders.so kaldi/decoders.cpp $(LIBFILE)
	rm -rf dist build *.egg-info

install: all
	# TODO I should distribute the shared library in more sensible way (ANY SUGGESTIONS?)
	cp -f $(LIBFILE) /usr/lib
	python setup.py install
	cd pyfst
	python setup.py install
	cd ..


cleanall: clean
	# recursive make clean
	rm -rf pyfst
