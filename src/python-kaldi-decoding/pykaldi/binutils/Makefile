include ../Makefile

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
PIPELINE_CONFIG=configs/decoding_pipeline_voxforge_config.json


test: run_pipeline_example kaldi_decoders_test.py



# PYTHON PIPELINE EXAMPLE
decoding_pipeline_example.py: $(BIGLIB) $(DATA_FILE) $(PIPELINE_CONFIG) $(WAVS_TRN) $(INPUT_SCP) $(UTT2SPK)
	echo $(WAVS) $(WAVS_TRN)

run_pipeline_example: decoding_pipeline_example.py
ifneq ($(KALDI_FLAVOR), dynamic)
	-echo "For C/Python interface we need shared library"
	-echo "The following test will probably fail!"
	-echo "Use configure with --shared flag!"
endif
	python $< $(PIPELINE_CONFIG)


$(DATA_TAR):
	wget -T 10 -t 3 "$(DATA_URL)"


$(DATA_FILE): $(DATA_TAR)
	tar xf $@.tar.bz2

$(DECODE_DIR):
	mkdir -p $@
	
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
pyclean: clean
	rm -f *.pyc 
	rm -rf $(DATA_TAR) $(DATA_FILE)

.PHONY: run_pipeline_example pytest
