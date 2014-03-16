#!/bin/bash

# source the settings
. path.sh

. utils/parse_options.sh || exit 1

decwrapdir=../../../dec-wrap
export LD_LIBRARY_PATH=$decwrapdir:$LD_LIBRARY_PATH
export PATH=$decwrapdir:$PATH

pushd $decwrapdir
    make dec-wrap-latgen-wrapper-test
popd

# dec-wrap-latgen-wrapper-test $wav_scp $batch_size $recogniser_latgen_tra \
#     --verbose=0  --max-mem=500000000 --lat-lm-scale=15 --config=$MFCC \
#     --beam=$beam --lattice-beam=$latbeam --max-active=$max_active \
#     $AM $HCLG `cat $SILENCE` $MAT

dec-wrap-latgen-wrapper-test ./data/vystadial-sample-cs/test/vad-2013-06-08-22-50-01.897179.wav \
    --verbose=0  --max-mem=500000000 --lat-lm-scale=15 --config=$MFCC \
    --beam=$beam --lattice-beam=$latbeam --max-active=$max_active \
    $AM $HCLG `cat $SILENCE` $MAT
