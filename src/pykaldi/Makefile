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
ADDLIBS = ../dec-wrap/dec-wrap.a ../decoder/kaldi-decoder.a \
		  ../thread/kaldi-thread.a ../lat/kaldi-lat.a \
		  ../hmm/kaldi-hmm.a ../transform/kaldi-transform.a ../gmm/kaldi-gmm.a \
		../fstext/kaldi-fstext.a ../tree/kaldi-tree.a ../matrix/kaldi-matrix.a \
		../feat/kaldi-feat.a ../util/kaldi-util.a ../base/kaldi-base.a



# ifeq ($(KALDI_FLAVOR), dynamic)
# dynamic compilation
LIBFILE=lib$(LIBNAME).so
$(LIBFILE): $(ADDLIBS)
	# Building shared library from static librariess.
	# The static libraries were compiled with -fPIC.
	$(CXX) -shared -DPIC -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(EXTRA_LDLIBS) $(LDFLAGS) $(LDLIBS)
# else
# # TODO
# # static compilation
# LIBFILE=$(LIBNAME).a
# endif

pyfst:
	git clone https://github.com/UFAL-DSG/pyfst.git pyfst

pyfst/fst/_fst.so: pyfst
	export FST=`pwd`/../../tools/openfst; cd pyfst ; LIBRARY_PATH=$$FST/lib:$$FST/lib/fst CPLUS_INCLUDE_PATH=$$FST/include python setup.py build_ext --inplace

pyfst_compiled: pyfst/fst/_fst.so

pykaldi/decoders.so: $(LIBFILE) pyfst_compiled
	# LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	# 			  python setup.py build_ext --inplace
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py develop --user

all: pykaldi/decoders.so

test: $(LIBFILE)
	LD_LIBRARY_PATH=.:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py nosetests

clean:
	@# removing build directory
	-python setup.py clean --all
	rm -f pykaldi/decoders.so pykaldi/decoders.cpp
	rm -rf dist build *.egg-info

cleanall: clean
	# recursive make clean
	$(MAKE) -C pykaldi clean
	$(MAKE) -C pykaldi/binutils clean
	$(MAKE) -C pykaldi/audio clean
