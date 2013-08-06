LIBNAME=pykaldi
LIBFILE=lib$(LIBNAME).so
SOURCEDIR=../dec-wrap

$(LIBFILE):
	$(MAKE) -C $(SOURCEDIR) $(LIBFILE)
	# TODO how to use the LIBFILE -> add it as dependancy to setup.py?
	cp $(SOURCEDIR)/$(LIBFILE) .

all: $(LIBFILE)

clean:
	@# removing build directory
	python setup.py clean --all
	rm -rf dist
	@# Do not want to use wildcards and rm -rf
	rm -f *.egg-info/* *.egg-info/.[a-zA-Z]*
	rmdir *.egg-info
	

cleanall:
	# TODO recursive make clean
