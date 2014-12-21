FSTDIR=$(PWD)/kaldi/tools/openfst
PYTHON=python2.7
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
	git submodule sync -- kaldi
	git submodule update kaldi

kaldi/src/kaldi.mk: kaldi/.git 
	@echo "kaldi configure"
	cd kaldi/src && ./configure --shared


kaldi/tools/ATLAS/include/clapack.h: kaldi/tools/Makefile
	@echo "Installing Atlas headers"
	cd kaldi/tools && make atlas

kaldi/tools/openfst/lib/libfst.a: kaldi/tools/Makefile
	@echo "Installing openfst locally"
	cd kaldi/tools && make openfst

pyfst/.git: .gitmodules
	git submodule sync -- pyfst
	git submodule update pyfst


# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so:  kaldi/tools/openfst/lib/libfst.a pyfst/.git
	cd pyfst ; \
		LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) \
		setup.py build_ext --inplace

pykaldi/kaldi/decoders.so: pyfst/fst/_fst.so onl-rec/libonl-rec.a
	cd pykaldi && \
	PYKALDI_ADDLIBS="../onl-rec/libonl-rec.a $(KALDI_LIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	$(PYTHON) setup.py build_ext --inplace


# test: all
# 	PYKALDI_ADDLIBS="$(ADDLIBS)" \
# 	LD_LIBRARY_PATH=./kaldi:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
# 	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
# 	PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
# 	$(PYTHON) setup.py nosetests

# install: all
# 	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) setup.py install ; cd .. && \
# 	PYKALDI_ADDLIBS="$(ADDLIBS)" \
# 	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
# 	$(PYTHON) setup.py install
#
#
# deploy: test "pykaldi_$(LINUX).tar.gz"
#
# "pykaldi_$(LINUX).tar.gz":
# 	PYKALDI_ADDLIBS="$(ADDLIBS)" \
# 	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
# 	PYKALDI_VERSION=$(PYKALDI_VERSION) $(PYTHON) setup.py bdist_egg
# 	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include $(PYTHON) setup.py bdist_egg 
# 	mkdir -p "pykaldi_$(LINUX)"/openfst
# 	cp dist/pykaldi*.egg pyfst/dist/pyfst*.egg "pykaldi_$(LINUX)"
# 	for d in include lib bin ; do cp -r $d $(FSTDIR)/$$d  "pykaldi_$(LINUX)"/openfst ; done
# 	tar czf "$@" "pykaldi_$(LINUX)"


distclean: clean
	# recursive make clean
	-cd kaldi/tools && make distclean 
	-cd kaldi/src && make clean; 
	-cd onl-rec && make distclean
	echo todo pyfst and pykaldi
	echo todo clean o
