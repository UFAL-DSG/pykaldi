# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
CONFIG = configs/voxforge_config.json
# CONFIG = configs/vystadial_config.json

# extracts testing data 
DECODE_DIR = work
include ../online-data.mk

all: decoders

decoders:
	$(MAKE) -C .. libpykaldi.so

test: run_online 

# PYTHON PIPELINE EXAMPLE
run_online: online_decode.py decoders $(DATA_TAR) $(CONFIG) 
	python $< $(CONFIG)

DATA = $(PWD)/online-data/models/tri2a
run_live: decoders $(DATA_TAR)
	python live_demo.py --rt-min=0.5 --rt-max=1.0 \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

	
clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc 

.PHONY: run_pipeline_example test clean decoders
