# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
DATA_FILE = online-data
DATA_URL = http://sourceforge.net/projects/kaldi/files/online-data.tar.bz2
DATA_TAR = $(DATA_FILE).tar.bz2
# MODEL_ML=$(DATA_FILE)/models/tri2a # MODEL_DT=$(DATA_FILE)/models/tri2b_mmi # AUDIO=$(DATA_FILE)/audio
DECODE_DIR = work
INPUT_SCP = $(DECODE_DIR)/input.scp
SPK2UTT = $(DECODE_DIR)/spk2utt
PIPELINE_CONFIG = configs/decoding_pipeline_voxforge_config.json
BIGLIB = libpykaldi.so

# Compiling the $(BIGLIB) in python-kaldi-decoding directory
$(BIGLIB):
	$(MAKE) -C ../..  $@
	cp ../../$@ .

test: run_pipeline_example

# PYTHON PIPELINE EXAMPLE
run_pipeline_example: decoding_pipeline_example.py $(BIGLIB) $(DATA_TAR) $(PIPELINE_CONFIG)
	python $< $(PIPELINE_CONFIG)


$(DATA_TAR):
	# generate helper files from the downloaded data
	wget -T 10 -t 3 "$(DATA_URL)"
	tar --keep-newer-files -xf $(DATA_TAR) > /dev/null 2> /dev/null
	mkdir -p $(DECODE_DIR)
	rm -f $(INPUT_SCP)  # reset the file do not append
	for f in $(DATA_FILE)/audio/*.wav; do \
		bf=`basename $$f`; bf=$${bf%.wav}; echo $$bf $$f >> $(INPUT_SCP); \
		echo $$bf $$bf >> $(SPK2UTT); \
		grep $$bf $(DATA_FILE)/audio/trans.txt | cut -d' ' -f 2- > $$f.trn; \
	done


$(DECODE_DIR):
	if [ ! -d $(DECODE_DIR) ]; then mkdir $(DECODE_DIR) ; fi
	
# HELPER functions
clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) $(BIGLIB) *.pyc 

.PHONY: run_pipeline_example test clean
