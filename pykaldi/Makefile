all: kaldi/decoders.so

FSTDIR=$(PWD)/../tools/openfst
LINUX=$(shell lsb_release -d | cut -f 2| sed 's:  *:_:g')

ADDLIBS = ../src/onl-rec/onl-rec.a ../src/online2/kaldi-online2.a \
		  ../src/decoder/kaldi-decoder.a ../src/feat/kaldi-feat.a \
		  ../src/thread/kaldi-thread.a ../src/lat/kaldi-lat.a ../src/hmm/kaldi-hmm.a \
		  ../src/transform/kaldi-transform.a ../src/gmm/kaldi-gmm.a ../src/fstext/kaldi-fstext.a \
		  ../src/tree/kaldi-tree.a ../src/matrix/kaldi-matrix.a  ../src/util/kaldi-util.a ../src/base/kaldi-base.a \
		  ../src/ivector/kaldi-ivector.a ../src/cudamatrix/kaldi-cudamatrix.a ../src/nnet2/kaldi-nnet2.a 

../src/onl-rec/onl-rec.a : ../tools/openfst/lib/libfst.so ../tools/ATLAS/include/clapack.h ../src/kaldi.mk
	@echo Building src dictionary namely $(ADDLIBS)
	@cd ../src ; for f in $(ADDLIBS) ; do make `echo $$f | sed 's:../src/\([a-z\-]*\)/.*:\1:'` ; done 


../tools/ATLAS/include/clapack.h: 
	@echo "Installing Atlas headers"
	cd ../tools && make atlas

../tools/openfst/lib/libfst.so:
	@echo "Installing openfst locally"
	cd ../tools && make openfst

../src/kaldi.mk:
	@echo "Preparing Kaldi makefile"
	cd ../src ; ./configure --shared

pyfst:
	git clone https://github.com/UFAL-DSG/pyfst.git pyfst

# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so: pyfst ../tools/openfst/lib/libfst.so
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include python setup.py build_ext --inplace ; cd ..

pyfst_compiled: pyfst/fst/_fst.so

kaldi/decoders.so: pyfst/fst/_fst.so $(ADDLIBS) kaldi/py_online_latgen_recogniser.pyx kaldi/py_online_nnet_latgen_recogniser.pyx 
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py build_ext --inplace


test: all
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LD_LIBRARY_PATH=./kaldi:$(FSTDIR)/lib:$(FSTDIR)/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
	python setup.py nosetests

clean:
	@# removing build directory
	-python setup.py clean --all
	rm -f kaldi/decoders.so kaldi/decoders.cpp
	rm -rf dist build *.egg-info

install: all
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include python setup.py install ; cd .. && \
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py install

deploy: test "pykaldi_$(LINUX).tar.gz"

"pykaldi_$(LINUX).tar.gz":
	mkdir -p "pykaldi_$(LINUX)"
	PYKALDI_ADDLIBS="$(ADDLIBS)" \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py bdist --dist-dir "pykaldi_$(LINUX)" --formats=tar
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include python setup.py bdist --dist-dir ../"pykaldi_$(LINUX)" --formats=tar ; cd ..
	mkdir -p "pykaldi_$(LINUX)"/openfst
	for d in include lib bin ; do cp -r $d $(FSTDIR)/$$d  "pykaldi_$(LINUX)"/openfst ; done
	tar czf "$@" "pykaldi_$(LINUX)"


distclean: clean
	# recursive make clean
	cd ../tools && make distclean 
	-cd ../src && make clean; 
	rm -rf pyfst "pykaldi_$(LINUX).tar.gz" "pykaldi_$(LINUX)"
