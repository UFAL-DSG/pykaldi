# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# PYTHON PIPELINE EXAMPLE settings 
DATA_FILE = online-data
DATA_URL = http://sourceforge.net/projects/kaldi/files/online-data.tar.bz2
DATA_TAR = $(DATA_FILE).tar.bz2
# MODEL_ML=$(DATA_FILE)/models/tri2a # MODEL_DT=$(DATA_FILE)/models/tri2b_mmi # AUDIO=$(DATA_FILE)/audio
DECODE_DIR = work
INPUT_SCP = $(DECODE_DIR)/input.scp
UTT2SPK = $(DECODE_DIR)/spk2utt
WAVS = $(wildcard $(DATA_FILE)/audio/*.wav)
WAVS_TRN = ${WAVS:.wav=.wav.trn}
PIPELINE_CONFIG = configs/decoding_pipeline_voxforge_config.json
BIGLIB = libpykaldi.so

# Compiling the $(BIGLIB) in python-kaldi-decoding directory
$(BIGLIB):
	$(MAKE) -C ../..  $@
	cp ../../$@ .

test: run_pipeline_example

# PYTHON PIPELINE EXAMPLE
run_pipeline_example: decoding_pipeline_example.py $(BIGLIB) $(DATA_FILE) $(PIPELINE_CONFIG) $(WAVS_TRN) $(INPUT_SCP) $(UTT2SPK)
	python $< $(PIPELINE_CONFIG)


$(DATA_TAR):
	wget -T 10 -t 3 "$(DATA_URL)"


$(DATA_FILE): $(DATA_TAR)
	tar --keep-newer-files -xf $@.tar.bz2 > /dev/null 2> /dev/null

$(DECODE_DIR):
	if [ ! -d $(DECODE_DIR) ]; then mkdir $(DECODE_DIR) ; fi
	
$(INPUT_SCP): $(DECODE_DIR) $(DATA_FILE)
	rm -f $@  # reset the file do not append
	for f in $(DATA_FILE)/audio/*.wav; do \
		bf=`basename $$f`; bf=$${bf%.wav}; echo $$bf $$f >> $@; \
	done

$(UTT2SPK): $(DECODE_DIR) $(DATA_FILE)
	rm -f $@  # reset the file do not append
	for f in $(DATA_FILE)/audio/*.wav; do \
		bf=`basename $$f`; bf=$${bf%.wav}; echo $$bf $$bf >> $@; \
	done


$(DATA_FILE)/audio/%.wav.trn: $(DATA_FILE)/audio/%.wav 
		bf=`basename $<`; bf=$${bf%.wav}; \
		grep $$bf $(DATA_FILE)/audio/trans.txt | cut -d' ' -f 2- > $@ 



# HELPER functions
clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) $(BIGLIB) *.pyc 

.PHONY: run_pipeline_example test clean
