all:

#########################################
#  Download and extracts online-data   #
#########################################
# This is the same data like in voxforge online_demo.

# default values
INPUT_SCP = $(DECODE_DIR)/input.scp
SPK2UTT = $(DECODE_DIR)/spk2utt
DATA_URL = http://sourceforge.net/projects/kaldi/files/online-data.tar.bz2
DATA_FILE = online-data
DATA_TAR = $(DATA_FILE).tar.bz2

# MODEL_ML=$(DATA_FILE)/models/tri2a # MODEL_DT=$(DATA_FILE)/models/tri2b_mmi # AUDIO=$(DATA_FILE)/audio

$(DATA_TAR): 
	@# generate helper files from the downloaded data
	wget -T 10 -t 3 "$(DATA_URL)"
	@# DEBUG ONLY: if no Internet available use copy
	@# cp $(DATA_TAR)_donotdelete $(DATA_TAR) 
	mkdir -p $(DECODE_DIR)
	tar --keep-newer-files -xf $(DATA_TAR) > /dev/null 2> /dev/null
	rm -f $(INPUT_SCP)  # reset the file do not append
	for f in $(DATA_FILE)/audio/*.wav; do \
		bf=`basename $$f`; bf=$${bf%.wav}; echo $$bf "$$PWD/$$f" >> $(INPUT_SCP); \
		echo $$bf $$bf >> $(SPK2UTT); \
		grep $$bf $(DATA_FILE)/audio/trans.txt | cut -d' ' -f 2- > $$f.trn; \
	done
