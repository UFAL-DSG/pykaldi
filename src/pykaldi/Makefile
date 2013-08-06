# TODO delegate all the work to the Makefile in pykaldi/pykaldi
# fix the handling of the paths

MAKE=$(MAKE) -C pykaldi

clean:
	@# removing build directory
	python setup.py clean --all
	rm -rf dist
	@# Do not want to use wildcards and rm -rf
	rm -f *.egg-info/* *.egg-info/.[a-zA-Z]*
	rmdir *.egg-info
	

cleanall:
	# TODO recursive make clean
