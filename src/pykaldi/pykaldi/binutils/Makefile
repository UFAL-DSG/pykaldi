VYSTADIAL_SCP=vystadial-sample-test/data/test/input.scp

vorforge_data:
	 python -c "import pykaldi.utils; pykaldi.utils.get_voxforge_data()"

voxforge:
	echo "Broken: TODO"

$(VYSTADIAL_SCP):
	 python -c "import pykaldi.utils; pykaldi.utils.get_vystadial_data('../../../../egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample-test.tar.bz2')"

pykaldi-faster: $(VYSTADIAL_SCP)
	bash run_pykaldi-faster-decoder.sh

pykaldi-latgen-faster: $(VYSTADIAL_SCP)
	bash run_pykaldi-latgen-faster-decoder.sh

gmm-latgen-faster: $(VYSTADIAL_SCP)
	bash run_gmm-latgen-faster.sh


DATA = $(PWD)/online-data/models/tri2a
live: vorforge_data
	python live_demo.py \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

	
clean:
	rm -rf online-data online-data.tar.bz2 *.pyc work

.PHONY: run_live pykaldi gmm_latgen test clean
