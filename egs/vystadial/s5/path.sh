# Needed for "correct" sorting
export LC_ALL=C

export KALDI_ROOT=`pwd`/../../..
export PATH=$PWD/utils/:$KALDI_ROOT/src/bin:$KALDI_ROOT/tools/openfst/bin:$KALDI_ROOT/src/fstbin/:$KALDI_ROOT/src/gmmbin/:$KALDI_ROOT/src/featbin/:$KALDI_ROOT/src/lm/:$KALDI_ROOT/src/sgmmbin/:$KALDI_ROOT/src/sgmm2bin/:$KALDI_ROOT/src/fgmmbin/:$KALDI_ROOT/src/latbin/:$PWD:$PATH

srilm_bin=$KALDI_ROOT/tools/srilm/bin/
srilm_sub_bin=`find "$srilm_bin" -type d`
for d in $srilm_sub_bin ; do
    export PATH=$d:$PATH
done
