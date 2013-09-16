# include python-kaldi-decoding Makefile 
# since we are dependent on the code there

# FIXME todo addd run vystadial with selected model
test: run_voxforge

work:
	 python -c "import pykaldi.utils; pykaldi.utils.get_voxforge_data()" 

run_voxforge: online_decode.py work configs/voxforge_config.json
	python online_decode.py configs/voxforge_config.json

run_vystadial: online_decode.py work configs/vystadial_config.json
	python online_decode.py configs/vystadial_config.json


DATA = $(PWD)/online-data/models/tri2a
run_live: work
	python live_demo.py \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

	
clean:
	rm -rf online-data online-data.tar.bz2 *.pyc  __pycache__ work

.PHONY: run_live run_online test clean
