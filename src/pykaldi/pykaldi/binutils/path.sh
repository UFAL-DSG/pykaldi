#!/bin/sh

# data location
PWD=`pwd`
exp_dir=$PWD/vystadial-sample-test/exp/tri1
data_dir=$PWD/vystadial-sample-test/data/test
decode_dir=$exp_dir/decode

# IO parameters
# wav_scp=$data_dir/input_best.scp
wav_scp=$data_dir/input.scp

gmm_latgen_faster_tra=$decode_dir/gmm-latgen-faster.tra
gmm_latgen_faster_tra_txt=${gmm_latgen_faster_tra}.txt

pykaldi_faster_tra=$decode_dir/pykaldi-faster.tra.txt
pykaldi_latgen_tra=$decode_dir/pykaldi-latgen.tra  # FIXME not used
pykaldi_latgen_tra_txt=${pykaldi_faster_tra}.txt
lattice=$decode_dir/lat.gz
# common configs
mfcc_config=conf/mfcc.conf
decode_config=conf/decode.conf
model=$exp_dir/final.mdl
hclg=$exp_dir/graph/HCLG.fst
wst=$exp_dir/graph/words.txt
