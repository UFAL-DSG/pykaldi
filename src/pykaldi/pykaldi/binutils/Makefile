BEST_LINE=16
# LANG available alternatives cs|en
LANG=cs
MODEL_PREFIX_URL=http://vystadial.ms.mff.cuni.cz/download/alex/resources/asr/voip_$(LANG)/kaldi/
HCLG=HCLG.fst
AM=final.mdl
LDA_MAT=final.mat
WST=words.txt

$(HCLG):
	wget $(MODEL_PREFIX_URL)/$@

$(AM):
	wget $(MODEL_PREFIX_URL)/$@

$(LDA_MAT):
	wget $(MODEL_PREFIX_URL)/$@

$(WST):
	wget $(MODEL_PREFIX_URL)/$@

vystadial-sample-$(LANG).tar.gz:
	wget http://vystadial.ms.mff.cuni.cz/download/vystadial-sample/vystadial-sample-$(LANG).tar.gz

vystadial-sample-$(LANG)/README.rst: vystadial-sample-$(LANG).tar.gz
	tar xf $<
	touch $@

vystadial-sample-$(LANG)/test/input.scp: vystadial-sample-$(LANG)/README.rst
	 python -c "from pykaldi.utils import build_input_scp as b; b('vystadial-sample-$(LANG)/test')"

vystadial-sample-$(LANG)/test/input_best.scp: vystadial-sample-$(LANG)/test/input.scp
	sed -n '$(BEST_LINE)p' < $< > $@

build_scp: vystadial-sample-$(LANG)/test/input_best.scp vystadial-sample-$(LANG)/test/input.scp

download_models: $(HCLG) $(AM) $(LDA_MAT) $(WST)

pykaldi-latgen-faster: build_scp download_models
	bash run_pykaldi-latgen-faster-decoder.sh

gmm-latgen-faster: build_scp download_models
	bash run_gmm-latgen-faster.sh

live: download_models
	bash run_live-demo.sh
	
clean:
	rm -rf *.pyc $(HCLG) $(AM) $(LDA_MAT) $(WST) vystadial-sample-*

.PHONY: live pykaldi-latgen-faster gmm-latgen-faster clean
