#!/bin/bash
# Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
# based on egs/voxforge script created by  Vassil Panayotov Copyright 2012, Apache 2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
# WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
# MERCHANTABLITY OR NON-INFRINGEMENT.
# See the Apache 2 License for the specific language governing permissions and
# limitations under the License. #

# The vystadial data are specific by having following marks in transcriptions
# _INHALE_
# _LAUGH_ 
# _EHM_HMM_ 
# _NOISE_
# _EHM_HMM_
# _SIL_

renice 20 $$

source conf/train_conf.sh

locdata=data/local
locdict=$locdata/dict
local_arpa_lm=data/local/lm.arpa

mkdir -p $locdata

echo "=== Preparing the LM ..."
echo debugging_${ARPA_MODEL}_

if [[ ! -z "$ARPA_MODEL" ]] ; then
    cp -f $ARPA_MODEL $local_arpa_lm
    echo "Using predefined LM in arpa format: ${ARPA_MODEL}"
else
    echo "=== Building LM of order ${LM_ORDER}..."
    [ -z "$IRSTLM" ] && echo "Set IRSTLM env variable for building LM" && exit 1;
    cut -d' ' -f2- data/train/text | sed -e 's:^:<s> :' -e 's:$: </s>:' | \
        grep -v '_INHALE_\|_LAUGH_\|_EHM_HMM_\|_NOISE_' \
        > $locdata/lm_train.txt

    # Launching irstlm script; See tools/INSTALL for installing irstlm
    build-lm.sh -i "$locdata/lm_train.txt" -n ${LM_ORDER} -o "$locdata/lm_phone_${LM_ORDER}.ilm.gz"
    # Launching irstlm script; See tools/INSTALL for installing irstlm
    compile-lm "$locdata/lm_phone_${LM_ORDER}.ilm.gz" --text $local_arpa_lm
fi

if [ ! -z "${NOOOV}" ]; then
  # NOT ALLOWING OOV WORDS training & also in decoding
  echo; echo "REMOVING OOV WORD FROM LANGUAGE MODEL"; echo
  pushd data/local
  grep -v -w OOV lm.arpa > lm.arpa_NO_OOV 
  mv lm.arpa_NO_OOV lm.arpa
  popd
else
  echo; echo "KEEPING OOV WORD IN LANGUAGE MODEL"; echo
fi


if [ ! -z "${DICTIONARY}" ]; then
  echo "Using predefined dictionary: ${DICTIONARY}"
  echo '</s>' > $locdata/vocab-full.txt
  tail -n +3 $DICTIONARY | cut -f 1 |\
    sort -u >> $locdata/vocab-full.txt 
else 
  # Do uppercasing but the data should be uppercased already
  cut -d' ' -f2- data/train/text | tr ' ' '\n' | \
    grep -v '_' | PERLIO=:utf8 perl -pe '$_=uc' | \
    sort -u > $locdata/vocab-full.txt
fi


echo "=== Preparing the dictionary ..."

if [ "$data_lang" == "en" ] ; then
    local/prepare_cmu_dict.sh $locdata $locdict
elif [ "$data_lang" == "cs" ] ; then
    local/run_cs_transcriptions.sh $locdata $locdict
else 
    echo "Unknown language $data_lang" ; exit 1
fi

# HANDLING OOV WORDS: OOV   SPN    UNKnow has pronancuation SPoken Noise
# Kaldi has special symbols SPN (Spoken Noise), NSN (Non Spoken Noise)
# and LAU (LAUGHTER)
echo "OOV SPN" > $locdict/lexicon-oov.txt
echo "_SIL_ NPN" >> $locdict/lexicon-oov.txt
echo "_INHALE_ NPN" >> $locdict/lexicon-oov.txt
echo "_LAUGH_ LAU" >> $locdict/lexicon-oov.txt
echo "_EHM_HMM_ NPN" >> $locdict/lexicon-oov.txt
echo "_NOISE_ NPN" >> $locdict/lexicon-oov.txt

cat $locdict/lexicon-oov.txt $locdict/lexicon-iv.txt |\
  sort > $locdict/lexicon.txt

echo "--- Prepare phone lists ..."
echo SIL > $locdict/silence_phones.txt
echo SIL > $locdict/optional_silence.txt

grep -v -w sil $locdict/lexicon.txt | \
  awk '{for(n=2;n<=NF;n++) { p[$n]=1; }} END{for(x in p) {print x}}' |\
  sort > $locdict/nonsilence_phones.txt

echo "--- Adding SIL to the lexicon ..."
echo -e "!SIL\tSIL" >> $locdict/lexicon.txt

# Some downstream scripts expect this file exists, even if empty
touch $locdict/extra_questions.txt

echo "*** Dictionary preparation finished!"

