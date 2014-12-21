
LINUX=$(shell lsb_release -d | cut -f 2| sed 's:  *:_:g')
PYKALDI_VERSION=0.1-dev
PYTHON=python2.7


all: kaldi/decoders.so






pyfst_compiled: pyfst/fst/_fst.so

kaldi/decoders.so: pyfst/fst/_fst.so $(ADDLIBS)
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	$(PYTHON) setup.py build_ext --inplace


test: all
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LD_LIBRARY_PATH=./kaldi:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
	$(PYTHON) setup.py nosetests

clean:
	@# removing build directory
	-$(PYTHON) setup.py clean --all
	rm -f kaldi/decoders.so kaldi/decoders.cpp
	rm -rf dist build *.egg-info
