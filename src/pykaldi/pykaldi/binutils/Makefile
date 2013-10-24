VYSTADIAL_SCP=vystadial-sample-test/data/test/input.scp

test: run_voxforge

vorforge_data:
	 python -c "import pykaldi.utils; pykaldi.utils.get_voxforge_data()"

voxforge: 
	echo "Broken: TODO"

$(VYSTADIAL_SCP):
	 python -c "import pykaldi.utils; pykaldi.utils.get_vystadial_data('/net/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample-test.tar.bz2')"

pykaldi_faster: $(VYSTADIAL_SCP)
	./run_pykaldi-faster-decoder.sh

pykaldi_latgen_faster: $(VYSTADIAL_SCP)
	./run_pykaldi-latgen-faster-decoder.sh

gmm_latgen: $(VYSTADIAL_SCP)
	./run_gmm_latgen_faster.sh


DATA = $(PWD)/online-data/models/tri2a
run_live: vorforge_data
	python live_demo.py \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

	
clean:
	rm -rf online-data online-data.tar.bz2 *.pyc  __pycache__ work

.PHONY: run_live pykaldi gmm_latgen test clean
