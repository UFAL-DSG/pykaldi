#!/bin/bash

# source the settings
. path.sh

. utils/parse_options.sh || exit 1


# cgdb -q -x .gdbinit_latgen --args \

onl-rec-latgen-recogniser-demo scp:$wav_scp $WST \
    --verbose=0  --max-mem=500000000 --config=$MFCC \
    --beam=$beam --lattice-beam=$latbeam --max-active=$max_active \
    $AM $HCLG `cat $SILENCE` $MAT > $kaldi_latgen_tra 

# reference is named based on wav_scp
./build_reference.py $wav_scp $decode_dir
reference=$decode_dir/`basename $wav_scp`.tra

echo; echo "Reference"; echo
cat $reference
echo; echo "Decoded"; echo
cat $kaldi_latgen_tra
echo

compute-wer --text --mode=present ark:$reference ark,p:$kaldi_latgen_tra

# echo; echo "Converting the lattice to svg picture ${wav_name}.svg" ; echo
# fstdraw --portrait=true --osymbols=$WST ${wav_name}.fst | dot -Tsvg  > ${wav_name}.svg
