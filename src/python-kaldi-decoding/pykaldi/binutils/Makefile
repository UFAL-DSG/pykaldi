# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
PIPELINE_CONFIG = configs/decoding_pipeline_voxforge_config.json
BIGLIB = libpykaldi.so

# extracts testing data 
DECODE_DIR = work
include ../online-data.mk


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
