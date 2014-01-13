BEST_LINE=16
# LANG available alternatives cs|en
LANG=cs
MODEL_PREFIX_URL=http://vystadial.ms.mff.cuni.cz/download/alex/resources/asr/voip_$(LANG)/kaldi/
HCLG=HCLG.fst
HCLG_LDA=HCLG_lda.fst
AM=final.mdl
AM_LDA=final_lda.mdl
MAT_LDA=final_lda.mat
WST=words.txt

$(HCLG):
	wget $(MODEL_PREFIX_URL)/$@

$(HCLG_LDA):
	wget $(MODEL_PREFIX_URL)/$@

$(AM):
	wget $(MODEL_PREFIX_URL)/$@

$(AM_LDA):
	wget $(MODEL_PREFIX_URL)/$@

$(MAT_LDA):
	wget $(MODEL_PREFIX_URL)/$@

$(WST):
	wget $(MODEL_PREFIX_URL)/$@

vystadial-sample-$(LANG).tar.gz:
	wget http://vystadial.ms.mff.cuni.cz/download/vystadial-sample/vystadial-sample-$(LANG).tar.gz

vystadial-sample-$(LANG)/README.rst: vystadial-sample-$(LANG).tar.gz
	tar xf $<
	touch $@

export BUILD_INPUT_SCP_PY

vystadial-sample-$(LANG)/test/input.scp: vystadial-sample-$(LANG)/README.rst
	for f in `dirname $@`/*.wav ; do echo "`basename $$f`" "`pwd`/$$f" ; done > $@
		
vystadial-sample-$(LANG)/test/input_best.scp: vystadial-sample-$(LANG)/test/input.scp
	sed -n '$(BEST_LINE)p' < $< > $@

build_scp: vystadial-sample-$(LANG)/test/input_best.scp vystadial-sample-$(LANG)/test/input.scp

download_models: $(HCLG) $(AM) $(WST) $(MAT_LDA) $(AM_LDA) $(HCLG_LDA)

pykaldi-latgen-faster: build_scp download_models
	bash run_pykaldi-latgen-faster-decoder.sh

gmm-latgen-faster: build_scp download_models
	bash run_gmm-latgen-faster.sh

live: download_models
	bash run_live-demo.sh
	
clean:
	rm -rf *.pyc $(HCLG) $(AM) $(MAT_LDA) $(WST) vystadial-sample-*

.PHONY: live pykaldi-latgen-faster gmm-latgen-faster clean
