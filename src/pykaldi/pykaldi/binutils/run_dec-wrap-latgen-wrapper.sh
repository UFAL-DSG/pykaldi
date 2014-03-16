#!/bin/bash

# source the settings
. path.sh

. utils/parse_options.sh || exit 1

decwrapdir=../../../dec-wrap
export LD_LIBRARY_PATH=$decwrapdir:$LD_LIBRARY_PATH
export PATH=$decwrapdir:$PATH

pushd $decwrapdir
    make dec-wrap-latgen-wrapper-test || exit 1
popd

# dec-wrap-latgen-wrapper-test $wav_scp $batch_size $recogniser_latgen_tra \
#     --verbose=0  --max-mem=500000000 --lat-lm-scale=15 --config=$MFCC \
#     --beam=$beam --lattice-beam=$latbeam --max-active=$max_active \
#     $AM $HCLG `cat $SILENCE` $MAT

# cgdb -q -x .gdbinit_latgen --args \

wav_name=./data/vystadial-sample-cs/test/vad-2013-06-08-22-50-01.897179.wav
dec-wrap-latgen-wrapper-test $wav_name \
    --verbose=0  --max-mem=500000000 --lat-lm-scale=15 --config=$MFCC \
    --beam=$beam --lattice-beam=$latbeam --max-active=$max_active \
    $AM $HCLG `cat $SILENCE` $MAT

echo; echo "Converting the lattice to svg picture ${wav_name}.svg" ; echo
fstdraw --portrait=true --osymbols=$WST ${wav_name}.fst | dot -Tsvg  > ${wav_name}.svg
