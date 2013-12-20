#!/bin/bash

# source the settings
. path.sh

batch_size=4560

date=`date +%Y-%m-%d`
tmp_config=/tmp/$(basename $0)_${date}.$$
cat $decode_config $mfcc_config > $tmp_config
echo 1>&2; echo Using config $tmp_config 1>&2 ; echo 1>&2
cat $tmp_config 1>&2 ; echo 1>&2

export LD_LIBRARY_PATH=`pwd`/../../:$LD_LIBRARY_PATH

# Below, there are various commands for debugging, profiling and always
# running the python wrapper around decoder. 
# Uncomment convenient prefix for you and put it just before the arguments. 
#
# cgdb -q -x .gdbinit_latgen --args python \
# kernprof.py -l -v  \
# valgrind --tool=callgrind -v --dump-instr=yes --trace-jump=yes --callgrind-out-file=callgrind.log python \
python \
pykaldi-latgen-faster-decoder.py $wav_scp $batch_size $pykaldi_latgen_tra $wst \
    --verbose=0 --lat-lm-scale=15 --config=$tmp_config \
    $model $hclg 1:2:3:4:5

# If using callgrind display the results by running kcachegrind
# kcachegrind callgrind.log

# reference is named based on wav_scp
./build_reference.py $wav_scp $decode_dir
reference=$decode_dir/`basename $wav_scp`.tra

echo; echo "Reference"; echo
cat $reference
echo; echo "Decoded"; echo
cat $pykaldi_latgen_tra

compute-wer --text --mode=present ark:$reference ark,p:$pykaldi_latgen_tra
