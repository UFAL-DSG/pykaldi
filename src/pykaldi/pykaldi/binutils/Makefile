test: run_voxforge

vorforge_data:
	 python -c "import pykaldi.utils; pykaldi.utils.get_voxforge_data()"

run_voxforge: online_decode.py vorforge_data configs/voxforge_config.json
	python online_decode.py configs/voxforge_config.json

vystadial-sample-test/data/test/input.scp:
	 python -c "import pykaldi.utils; pykaldi.utils.get_vystadial_data('/net/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample-test.tar.bz2')"

run_vystadial: online_decode.py vystadial-sample-test/data/test/input.scp configs/vystadial_config.json
	python online_decode.py configs/vystadial_config.json


DATA = $(PWD)/online-data/models/tri2a
run_live: vorforge_data
	python live_demo.py \
		--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 \
		$(DATA)/model $(DATA)/HCLG.fst $(DATA)/words.txt 1:2:3:4:5

	
clean:
	rm -rf online-data online-data.tar.bz2 *.pyc  __pycache__ work

.PHONY: run_live run_online test clean
