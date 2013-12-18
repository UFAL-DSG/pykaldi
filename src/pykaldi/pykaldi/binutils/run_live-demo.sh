#!/bin/bash

# source the settings
. path.sh

batch_size=4560

date=`date +%Y-%m-%d`
tmp_config=/tmp/$(basename $0)_${date}.$$
cat $decode_config $mfcc_config > $tmp_config
echo 1>&2; echo Using config $tmp_config 1>&2 ; echo 1>&2
cat $tmp_config 1>&2 ; echo 1>&2

export LD_LIBRARY_PATH=`pwd`/../../../dec-wrap:$LD_LIBRARY_PATH

python live-demo.py $batch_size $wst \
    --verbose=0 --lat-lm-scale=15 --config=$tmp_config \
    $model $hclg 1:2:3:4:5
