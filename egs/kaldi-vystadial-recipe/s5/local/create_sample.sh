#!/bin/bash
# example usage:
# ./local/create_sample.sh /ha/projects/vystadial/data/asr/en/voip/ Results/vystadial-sample/ test 50

src=$1
tgt=$2
typ=$3   # dev test train
n=$4

for d in test train dev ; do
    src_dir=$src/$typ
    tgt_dir=$tgt/$typ
    mkdir -p $tgt_dir
    ls $src_dir/*.wav | head -n $n \
    | while read f ; do
        cp $f $tgt_dir
        cp ${f}.trn $tgt_dir
    done
done
