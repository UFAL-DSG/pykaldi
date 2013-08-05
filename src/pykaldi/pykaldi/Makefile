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

# PYTHON PIPELINE EXAMPLE
run: binutils/online_decode.py decoders.py $(DATA_TAR) $(CONFIG) 
	PYTHONPATH=$(PWD)/.. python $< $(CONFIG)

DATA = $(PWD)/online-data/models/tri2a
run_live: binutils/live_demo.py decoders.py $(DATA_TAR)
	PYTHONPATH=$(PWD)/.. $< --rt-min=0.5 --rt-max=1.0 \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

# test: decoders_test.py
test: decoders_test.py $(DATA_TAR)
	nosetests -s $<  # -s let the output be printed

clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc

.PHONY: run_live test clean
