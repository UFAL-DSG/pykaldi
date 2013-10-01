#!/bin/bash

# source the settings
. path.sh

batch_size=4560

./pykaldi-faster-decoder.py $wav_scp $batch_size $pykaldi_faster_tra \
        --verbose=2 --acoustic-scale=0.1 --config=$decode_config \
        $model $hclg $wst 1:2:3:4:5

# reference is named based on wav_scp 
./build_reference.py $wav_scp $decode_dir  
reference=$decode_dir/`basename $wav_scp`.tra
compute-wer --text --mode=present ark:$reference ark,p:$pykaldi_faster_tra

echo; echo "Reference"; echo
cat $reference
echo; echo "Decoded"; echo
cat $pykaldi_faster_tra
