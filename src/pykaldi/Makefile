LIBNAME=pykaldi
LIBFILE=lib$(LIBNAME).so
SOURCEDIR=../dec-wrap


all:
	$(MAKE) -C $(SOURCEDIR) $(LIBFILE)
	python setup.py build_ext --inplace

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
