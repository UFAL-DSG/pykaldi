# TODO: installation of patched shared OpenFST library is stupid
FSTDIR=kaldi/tools/openfst
PYTHON=python2.7
LINUX=$(shell lsb_release -sd | sed 's: :_:g')
KALDI_LIBS = ../kaldi/src/online2/kaldi-online2.a \
		  ../kaldi/src/decoder/kaldi-decoder.a \
		  ../kaldi/src/feat/kaldi-feat.a \
		  ../kaldi/src/thread/kaldi-thread.a \
		  ../kaldi/src/lat/kaldi-lat.a \
		  ../kaldi/src/hmm/kaldi-hmm.a \
		  ../kaldi/src/transform/kaldi-transform.a \
		  ../kaldi/src/gmm/kaldi-gmm.a \
		  ../kaldi/src/fstext/kaldi-fstext.a \
		  ../kaldi/src/tree/kaldi-tree.a \
		  ../kaldi/src/matrix/kaldi-matrix.a \
		  ../kaldi/src/util/kaldi-util.a \
		  ../kaldi/src/base/kaldi-base.a

all: onl-rec/libonl-rec.a pykaldi/kaldi/decoders.so pyfst/fst/_fst.so

onl-rec/libonl-rec.a: $(FSTDIR)/lib/libfst.a kaldi/tools/ATLAS/include/clapack.h kaldi/src/kaldi.mk
	cd kaldi/src && \
		make
		# make online2 ivector decoder feat thread lat hmm transform gmm fstext tree matrix util base
	cd onl-rec && make

kaldi/.git: .gitmodules
	git submodule init
	git submodule sync -- kaldi
	git submodule update kaldi

pyfst/.git: .gitmodules
	git submodule init
	git submodule sync -- pyfst
	git submodule update pyfst

kaldi/src/kaldi.mk: kaldi/.git 
	@echo "kaldi configure"
	cd kaldi/src && ./configure --shared


kaldi/tools/ATLAS/include/clapack.h: kaldi/.git
	@echo "Installing Atlas headers"
	cd kaldi/tools && make atlas

$(FSTDIR)/lib/libfst.a: kaldi/.git 
	@echo "Installing openfst locally"
	cd kaldi/tools && make openfst

# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so:  $(FSTDIR)/lib/libfst.a pyfst/.git
	cd pyfst ; \
		LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) \
		setup.py build_ext --inplace

pykaldi/kaldi/decoders.so: pyfst/fst/_fst.so onl-rec/libonl-rec.a
	cd pykaldi && \
	PYKALDI_ADDLIBS="../onl-rec/libonl-rec.a $(KALDI_LIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	$(PYTHON) setup.py build_ext --inplace

distclean:
	-cd kaldi/tools && make distclean 
	-cd kaldi/src && make clean; 
	-cd onl-rec && make distclean
	-cd pykaldi && $(PYTHON) setup.py clean --all && rm -rf pykaldi/{dist,build,*e.egg-info}
	-cd pyfst && $(PYTHON) setup.py clean --all && rm -rf pyfst/{dist,build,*e.egg-info}

test: pykaldi/kaldi/decoders.so pyfst/fst/_fst.so
	cd onl-rec && \
	make test && \
	cd pykaldi && \
	PYKALDI_ADDLIBS="../onl-rec/libonl-rec.a $(KALDI_LIBS)" \
	LD_LIBRARY_PATH=./kaldi:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
	$(PYTHON) setup.py nosetests

install: pykaldi/kaldi/decoders.so pyfst/fst/_fst.so
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) setup.py install ; cd .. && \
	cd pykaldi && PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	$(PYTHON) setup.py install


deploy: pykaldi_$(LINUX).zip

pykaldi_$(LINUX).zip: pykaldi/kaldi/decoders.so pyfst/fst/_fst.so
	cd pykaldi && \
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	PYKALDI_VERSION=$(PYKALDI_VERSION) $(PYTHON) setup.py bdist_egg
	cd pyfst && LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) setup.py bdist_egg 
	mkdir -p "pykaldi_$(LINUX)"/openfst
	cp pykaldi/dist/pykaldi*.egg pyfst/dist/pyfst*.egg "pykaldi_$(LINUX)"
	for d in include lib bin ; do cp -r $d $(FSTDIR)/$$d  "pykaldi_$(LINUX)"/openfst ; done
	zip -r $@ $(basename $@)
