all: kaldi/decoders.so

FSTDIR=$(PWD)/../tools/openfst
LINUX=$(shell lsb_release -d | cut -f 2| sed 's:  *:_:g')

ADDLIBS = ../src/onl-rec/onl-rec.a ../src/decoder/kaldi-decoder.a ../src/feat/kaldi-feat.a \
		 ../src/thread/kaldi-thread.a ../src/lat/kaldi-lat.a ../src/hmm/kaldi-hmm.a \
		 ../src/transform/kaldi-transform.a ../src/gmm/kaldi-gmm.a ../src/fstext/kaldi-fstext.a \
		 ../src/tree/kaldi-tree.a ../src/matrix/kaldi-matrix.a  ../src/util/kaldi-util.a ../src/base/kaldi-base.a

pyfst:
	git clone https://github.com/UFAL-DSG/pyfst.git pyfst

# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so: pyfst
	cd pyfst ; LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include python setup.py build_ext --inplace

pyfst_compiled: pyfst/fst/_fst.so

kaldi/decoders.so: pyfst_compiled $(ADDLIBS)
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
	cd pyfst;  python setup.py install; cd ..
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


cleanall: clean
	# recursive make clean
	rm -rf pyfst "pykaldi_$(LINUX).tar.gz" "pykaldi_$(LINUX)"
