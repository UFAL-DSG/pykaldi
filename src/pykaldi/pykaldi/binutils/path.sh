#!/bin/bash

# data location
PWD=`pwd`
exp_dir=$PWD
data_dir=$PWD/data/vystadial-sample-cs/test
decode_dir=$exp_dir/decode

# IO parameters
wav_scp=$data_dir/input_best.scp
# wav_scp=$data_dir/input.scp

gmm_latgen_faster_tra=$decode_dir/gmm-latgen-faster.tra
gmm_latgen_faster_tra_txt=${gmm_latgen_faster_tra}.txt

pykaldi_latgen_tra=$decode_dir/pykaldi-latgen.tra
pykaldi_latgen_tra_txt=${pykaldi_faster_tra}.txt
lattice=$decode_dir/lat.gz

# Czech language models 
LANG=cs
HCLG=models/HCLG_tri2b_bmmi.fst
AM=models/tri2b_bmmi.mdl
MAT=models/tri2b_bmmi.mat  # matrix trained in tri2b models 
WST=models/words.txt
MFCC=models/mfcc.conf
SILENCE=models/silence.csl

pykaldi_dir=`pwd`/../..
openfstlib_dir=`pwd`/../../../../tools/openfst/lib
export LD_LIBRARY_PATH=$openfstlib_dir:$openfstlib_dir/fst:$pykaldi_dir/pykaldi:$LD_LIBRARY_PATH
export PYTHONPATH=$pykaldi_dir:$pykaldi_dir/pyfst:$PYTHONPATH

kaldisrc=`pwd`/../../..
export PATH=$kaldisrc/bin:$kaldisrc/fgmmbin:$kaldisrc/gmmbin:$kaldisrc/nnetbin:$kaldisrc/sgmm2bin:$kaldisrc/tiedbin:$kaldisrc/featbin:$kaldisrc/fstbin:$kaldisrc/latbin:$kaldisrc/onlinebin:$kaldisrc/sgmmbin:"$PATH"


beam=16.0
latbeam=10.0
max_active=14000

# Size of chunks are queued in "online" interface
batch_size=4560
