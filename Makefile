# TODO Cython does not recompile because of the templates and sourcefiles
FSTDIR=kaldi/tools/openfst
AFSTDIR=$(PWD)/$(FSTDIR)
PYTHON=python
INSTALL_PREFIX=/usr/local
INSTALL_PYTHON_PREFIX=/usr/local/
# OPENFST_VERSION=1.4.1 # fails with pyfst
OPENFST_VERSION=1.3.4
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

all: onl-rec/onl-rec.a pykaldi/kaldi/decoders.so pyfst/fst/_fst.so

kaldi/.git: .gitmodules
	git submodule init kaldi
	git submodule sync -- kaldi
	git submodule update kaldi

pyfst/.git: .gitmodules
	git submodule init pyfst
	git submodule sync -- pyfst
	git submodule update pyfst

kaldi/src/kaldi.mk: kaldi/.git $(FSTDIR)/lib/libfst.a kaldi/tools/ATLAS/include/clapack.h
	@echo "kaldi configure"
	cd kaldi/src && \
		./configure --shared

kaldi/tools/ATLAS/include/clapack.h: kaldi/.git
	$(MAKE) -C kaldi/tools  atlas ; echo "Installing atlas finished $?"

$(FSTDIR)/lib/libfst.a: kaldi/.git
	$(MAKE) -C kaldi/tools openfst OPENFST_VERSION=$(OPENFST_VERSION); echo "Installing OpenFST finished: $?"

# If you want to develop or install pyfst
# use setup.py develop --user or setup.py install respectively
pyfst/fst/_fst.so:  $(FSTDIR)/lib/libfst.a pyfst/.git
	cd pyfst && \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include $(PYTHON) \
		setup.py build_ext --inplace

onl-rec/onl-rec.a: $(FSTDIR)/lib/libfst.a kaldi/tools/ATLAS/include/clapack.h kaldi/src/kaldi.mk
	$(MAKE) -C kaldi/src
	# $(MAKE) -C kaldi/src online2 ivector decoder feat thread lat hmm transform gmm fstext tree matrix util base
	$(MAKE) -C onl-rec

pykaldi/kaldi/decoders.so: pyfst/fst/_fst.so onl-rec/onl-rec.a
	cd pykaldi && \
		PYKALDI_ADDLIBS="../onl-rec/onl-rec.a $(KALDI_LIBS)" \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include \
		$(PYTHON) setup.py build_ext --inplace

test: onl-rec/onl-rec.a $(FSTDIR)/lib/libfst.a pyfst/.git
	$(MAKE) -C onl-rec test
	cd pyfst && \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include \
		LD_LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst \
		DYLD_LIBRARY_PATH=$$LD_LIBRARY_PATH \
		$(PYTHON) setup.py nosetests
	cd pykaldi && \
		PYKALDI_ADDLIBS="../onl-rec/onl-rec.a $(KALDI_LIBS)" \
		LD_LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst \
		DYLD_LIBRARY_PATH=$$LD_LIBRARY_PATH \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include \
		PYTHONPATH=$(PWD)/pyfst:$$PYTHONPATH \
		$(PYTHON) setup.py nosetests

distclean:
	$(MAKE) -C kaldi/tools distclean
	$(MAKE) -C kaldi/src clean
	$(MAKE) -C onl-rec distclean
	cd pykaldi && \
		$(PYTHON) setup.py clean --all
	rm -rf pykaldi/{dist,build,*e.egg-info}
	rm -f kaldi/decoders.{cpp,so}
	cd pyfst && \
		$(PYTHON) setup.py clean --all
	rm -rf pyfst/{dist,build,*e.egg-info}
	rm -f fst/_fst.{cpp,so}

deploy: pykaldi_$(LINUX).zip

pykaldi_$(LINUX).zip: pykaldi/kaldi/decoders.so pyfst/fst/_fst.so
	cd pyfst && \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include \
		$(PYTHON) setup.py bdist_egg
	cd pykaldi && \
		PYKALDI_ADDLIBS="../onl-rec/onl-rec.a $(KALDI_LIBS)" \
		LIBRARY_PATH=$(AFSTDIR)/lib:$(AFSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(AFSTDIR)/include \
		$(PYTHON) setup.py bdist_egg
	mkdir -p $(basename $@)/openfst
	cp pykaldi/dist/pykaldi*.egg pyfst/dist/pyfst*.egg $(basename $@)
	for d in include lib bin ; do cp -r $(AFSTDIR)/$$d  $(basename $@)/openfst ; done
	zip -r $@ $(basename $@)

install-kaldi-binaries: kaldi/src/kaldi.mk
	cp -r kaldi/src/lib/* $(INSTALL_PREFIX)/lib
	cp `find kaldi/src -executable -type f` $(INSTALL_PREFIX)/bin

install: pykaldi_$(LINUX).zip
	# as one command due to setting up dir variable (TODO move it to make variable)
	export dir=`mktemp -d pykaldi_install_XXXXX`; \
	echo -e "\nInstalling from $$dir\n"; \
	mkdir -p $$dir; \
	cp $< $$dir; \
	cd $$dir && \
		unzip -q $<
		cd $(basename $<) && \
			for d in bin include lib ; do  cp -r openfst/$$d/* $(INSTALL_PREFIX)/$$d/ ; done ; \
			easy_install pyfst*.egg ; \
			easy_install pykaldi*.egg; \
	echo -e "\nRemoving $$dir\n" ; \
	rm -rf $$dir


irstlm:
	svn -r 769 co --non-interactive --trust-server-cert https://svn.code.sf.net/p/irstlm/code/trunk irstlm

irstlm/Makefile: irstlm
	cd irstlm && cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$(INSTALL_PREFIX)"

install-irstlm: irstlm/Makefile
	$(MAKE) -C irstlm
	$(MAKE) -C irstlm install
