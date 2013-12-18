VYSTADIAL_SCP=vystadial-sample-test/data/test/input.scp

$(VYSTADIAL_SCP):
	 python -c "import pykaldi.utils; pykaldi.utils.get_vystadial_data('../../../../egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample-test.tar.bz2')"

pykaldi-faster: $(VYSTADIAL_SCP)
	bash run_pykaldi-faster-decoder.sh

pykaldi-latgen-faster: $(VYSTADIAL_SCP)
	bash run_pykaldi-latgen-faster-decoder.sh

gmm-latgen-faster: $(VYSTADIAL_SCP)
	bash run_gmm-latgen-faster.sh

live: $(VYSTADIAL_SCP)
	bash run_live-demo.sh
	
clean:
	rm -rf online-data online-data.tar.bz2 *.pyc work

.PHONY: run_live pykaldi gmm_latgen test clean
