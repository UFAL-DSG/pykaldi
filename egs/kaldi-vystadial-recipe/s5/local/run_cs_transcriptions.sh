#!/bin/bash

source conf/train_conf.sh

locdata=$1; shift
locdict=$1; shift


if [ ! -z "${DICTIONARY}" ]; then
  echo "Using predefined dictionary: ${DICTIONARY}"
  echo '</s>' > $locdata/vocab-full.txt
  tail -n +3 $DICTIONARY | cut -f 1 |\
    sort -u >> $locdata/vocab-full.txt 
else 
  cut -d' ' -f2- data/train/text | tr ' ' '\n' | sort -u > $locdata/vocab-full.txt
fi

echo "The results are stored in $locdata/vocab-full.txt"
echo "Should be utf-8 and words in CAPITALS"
echo "CHECK the ENCODING!"; echo


mkdir -p $locdict 

perl local/phonetic_transcription_cs.pl $locdata/vocab-full.txt $locdict/cs_transcription.txt


echo "--- Searching for OOV words ..."
gawk 'NR==FNR{words[$1]; next;} !($1 in words)' \
  $locdict/cs_transcription.txt $locdata/vocab-full.txt |\
  egrep -v '<.?s>' > $locdict/vocab-oov.txt

gawk 'NR==FNR{words[$1]; next;} ($1 in words)' \
  $locdata/vocab-full.txt $locdict/cs_transcription |\
  egrep -v '<.?s>' > $locdict/lexicon-iv.txt

wc -l $locdict/vocab-oov.txt
wc -l $locdict/lexicon-iv.txt
