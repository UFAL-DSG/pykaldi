LIBNAME=pykaldi
LIBFILE=lib$(LIBNAME).so
SOURCEDIR=../dec-wrap
FSTDIR=$(PWD)/../../tools/openfst

all:
	$(MAKE) -C $(SOURCEDIR) $(LIBFILE)
	# LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	# 			  python setup.py build_ext --inplace
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py develop --user

test:
	LD_LIBRARY_PATH=$(SOURCEDIR):../../tools/openfst/lib:../../tools/openfst/lib/fst \
	LIBRARY_PATH=$(FSTDIR)/lib:$(FSTDIR)/lib/fst CPLUS_INCLUDE_PATH=$(FSTDIR)/include \
	python setup.py nosetests

clean:
	@# removing build directory
	-python setup.py clean --all
	rm -rf dist
	@# Do not want to use wildcards and rm -rf
	rm -f *.egg-info/* *.egg-info/.[a-zA-Z]* build
	-rmdir *.egg-info

cleanall: clean
	# recursive make clean
	$(MAKE) -C pykaldi clean
	$(MAKE) -C pykaldi/binutils clean
	$(MAKE) -C pykaldi/audio clean
