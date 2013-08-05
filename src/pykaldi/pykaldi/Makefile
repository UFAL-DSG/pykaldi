LIBNAME=pykaldi
LIBFILE=lib$(LIBNAME).so
SOURCEDIR=../../dec-wrap

CONFIG = binutils/configs/voxforge_config.json
# CONFIG = binutils/configs/vystadial_config.json


$(LIBFILE):
	$(MAKE) -C $(SOURCEDIR) $(LIBFILE)
	cp $(SOURCEDIR)/$(LIBFILE) .
	
__init__.py: $(LIBFILE)

decoders.py: __init__.py

decoders_test.py: __init__.py

# extracts testing data
DECODE_DIR = work
include ../online-data.mk

# test: decoders_test.py
test: decoders_test.py $(DATA_TAR)
	nosetests -s $<  # -s let the output be printed

clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc

.PHONY: run_live test clean
