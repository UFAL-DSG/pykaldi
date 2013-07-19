# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
VOXFORGE_CONFIG = configs/voxforge_config.json

# extracts testing data 
DECODE_DIR = work
include ../online-data.mk

all: decoders

decoders:
	$(MAKE) -C .. libpykaldi.so

test: run_online 

# PYTHON PIPELINE EXAMPLE
run_online: online_decode.py decoders $(DATA_TAR) $(VOXFORGE_CONFIG) 
	python $< $(VOXFORGE_CONFIG)

run_live: decoders
	echo "BROKEN!"
	python live_demo.py

	
clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc 

.PHONY: run_pipeline_example test clean decoders
