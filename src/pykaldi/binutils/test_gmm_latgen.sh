#!/bin/sh

exp_dir=/ha/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/exp/tri2a
data_dir=/ha/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/data/test/split10/1
# diference: 
feats_scp=$data_dir/feats.scp
# online_needs: here unused
wav_scp=$data_dir/wav.scp
cmvn_scp=$data_dir/cmvn.scp
utt2spk=$data_dir/utt2spk

gmm_config=configs/decode.config
model=$exp_dir/final.mdl
hclg=$exp_dir/graph/HCLG.fst
wst=$exp_dir/graph/words.txt

# out params=
lattice=$PWD/work/lat.gz

gmm-latgen-faster --config=configs/decode.config  \
    --word-symbol-table=$wst $model $hclg \
    "ark,s,cs:apply-cmvn --norm-vars=false --utt2spk=ark:$utt2spk scp:$cmvn_scp scp:$feats_scp ark:- | add-deltas  ark:- ark:- |" \
    "ark:|gzip - c > $lattice"

lattice-best-path --lm-scale=15 --word-symbol-table=$wst "ark:gunzip -c $lattice|" \
    ark,t:work/bash.trans
