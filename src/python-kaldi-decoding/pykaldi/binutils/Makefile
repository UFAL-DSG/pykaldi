# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
PIPELINE_CONFIG = configs/decoding_pipeline_voxforge_config.json
BIGLIB = libpykaldi.so

# it adds only one target: DATA_TAR for obtaining test data
include ../online-data.mk
# params for online-data.mk
DECODE_DIR = work
INPUT_SCP = $(DECODE_DIR)/input.scp
SPK2UTT = $(DECODE_DIR)/spk2utt


# Compiling the $(BIGLIB) in python-kaldi-decoding directory
$(BIGLIB):
	$(MAKE) -C ../..  $@
	cp ../../$@ .

all: $(BIGLIB)

test: run_pipeline_example

# PYTHON PIPELINE EXAMPLE
run_pipeline_example: decoding_pipeline_example.py $(BIGLIB) $(DATA_TAR) $(PIPELINE_CONFIG) 
	python $< $(PIPELINE_CONFIG)


	
clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) $(BIGLIB) *.pyc 

.PHONY: run_pipeline_example test clean
