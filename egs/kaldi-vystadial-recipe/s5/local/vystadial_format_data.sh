#!/bin/bash

# Copyright 2012 Vassil Panayotov
#           2013 Ondrej Platek
# Apache 2.0

source ./path.sh
source ./conf/train_conf.sh

echo "=== Formating data ..."
srcdir=data/local
lmdir=data/local/
tmpdir=data/local/lm_tmp
lexicon=data/local/dict/lexicon.txt
local_arpa_lm0=data/local/lm0.arpa
test_lm="DEFAULT_should_be_set_in_script"

# Next, for each type of language model, create the corresponding FST
# and the corresponding lang_test_* directory.


for t in $test_sets ; do
 mkdir -p $tmpdir
 test=data/lang_$t
 echo "--- Preparing the grammar transducer (G.fst) for $t in $test..."
 mkdir -p $test
 for f in phones.txt words.txt phones.txt L.fst L_disambig.fst phones/; do
     cp -r data/lang/$f $test
 done
 cat $lmdir/lm.arpa | \
    utils/find_arpa_oovs.pl $test/words.txt > $tmpdir/oovs.txt
 
 if [[ -z "$TEST_ZERO_GRAMS" ]]; then
  echo "=== Test data will use LM of order {LM_ORDER}"
  test_lm=$lmdir/lm.arpa
 else
  echo "=== Building ZERO GRAM for testing data..."
  [ -z "$IRSTLM" ] && echo "Set IRSTLM env variable for building LM" && exit 1;
  cut -d' ' -f2- data/test/text | sed -e 's:^:<s> :' -e 's:$: </s>:' | \
      grep -v '_INHALE_\|_LAUGH_\|_EHM_HMM_\|_NOISE_' \
      > $lmdir/lm_test.txt

  # Launching irstlm script; See tools/INSTALL for installing irstlm
  build-lm.sh -i "$lmdir/lm_test.txt" -n 0 -o "$lmdir/lm_phone_zero.ilm.gz"
  # Launching irstlm script; See tools/INSTALL for installing irstlm
  compile-lm "$lmdir/lm_phone_zero.ilm.gz" --text $local_arpa_lm0

  test_lm=$local_arpa_lm0
 fi
 # grep -v '<s> <s>' because the LM seems to have some strange and useless
 # stuff in it with multiple <s>'s in the history.  Encountered some other similar
 # things in a LM from Geoff.  Removing all "illegal" combinations of <s> and </s>,
 # which are supposed to occur only at being/end of utt.  These can cause 
 # determinization failures of CLG [ends up being epsilon cycles].
 cat $test_lm | \
   grep -v '<s> <s>' | \
   grep -v '</s> <s>' | \
   grep -v '</s> </s>' | \
   arpa2fst - | fstprint | \
   utils/remove_oovs.pl $tmpdir/oovs.txt | \
   utils/eps2disambig.pl | utils/s2eps.pl | fstcompile --isymbols=$test/words.txt \
     --osymbols=$test/words.txt  --keep_isymbols=false --keep_osymbols=false | \
   fstrmepsilon > $test/G.fst
 fstisstochastic $test/G.fst
 # The output is like:
 # 9.14233e-05 -0.259833
 # we do expect the first of these 2 numbers to be close to zero (the second is
 # nonzero because the backoff weights make the states sum to >1).
 # Because of the <s> fiasco for these particular LMs, the first number is not
 # as close to zero as it could be.
 
 # Everything below is only for diagnostic.
 # Checking that G has no cycles with empty words on them (e.g. <s>, </s>);
 # this might cause determinization failure of CLG.
 # #0 is treated as an empty word.
 mkdir -p $tmpdir/g
 awk '{if(NF==1){ printf("0 0 %s %s\n", $1,$1); }} END{print "0 0 #0 #0"; print "0";}' \
   < "$lexicon"  >$tmpdir/g/select_empty.fst.txt
 fstcompile --isymbols=$test/words.txt --osymbols=$test/words.txt \
   $tmpdir/g/select_empty.fst.txt | \
 fstarcsort --sort_type=olabel | fstcompose - $test/G.fst > $tmpdir/g/empty_words.fst
 fstinfo $tmpdir/g/empty_words.fst | grep cyclic | grep -w 'y' && 
   echo "Language model has cycles with empty words" && exit 1
 rm -rf $tmpdir
 
 echo "*** Succeeded in formatting data for $test"

done # for test in $test_sets
