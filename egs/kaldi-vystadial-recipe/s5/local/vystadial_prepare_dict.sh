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

echo "=== Preparing the dictionary ..."

if [ ! -z "${DICTIONARY}" ]; then
    echo "Using predefined dictionary: ${DICTIONARY}"
    echo '</s>' > $locdata/vocab-full.txt
    tail -n +3 $DICTIONARY | cut -f 1 |\
      sort | uniq >> $locdata/vocab-full.txt 
else 
    cut -d' ' -f2- data/train/text | tr ' ' '\n' | sort -u > $locdata/vocab-full.txt
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


if [ ! -f $locdict/cmudict/cmudict.0.7a ]; then
  echo "--- Downloading CMU dictionary ..."
  mkdir -p $locdict 
  svn co http://svn.code.sf.net/p/cmusphinx/code/trunk/cmudict \
    $locdict/cmudict || exit 1;
fi

echo "--- Striping stress and pronunciation variant markers from cmudict ..."
perl $locdict/cmudict/scripts/make_baseform.pl \
  $locdict/cmudict/cmudict.0.7a /dev/stdout |\
  sed -e 's:^\([^\s(]\+\)([0-9]\+)\(\s\+\)\(.*\):\1\2\3:' > $locdict/cmudict-plain.txt

echo "--- Searching for OOV words ..."
gawk 'NR==FNR{words[$1]; next;} !($1 in words)' \
  $locdict/cmudict-plain.txt $locdata/vocab-full.txt |\
  egrep -v '<.?s>' > $locdict/vocab-oov.txt

gawk 'NR==FNR{words[$1]; next;} ($1 in words)' \
  $locdata/vocab-full.txt $locdict/cmudict-plain.txt |\
  egrep -v '<.?s>' > $locdict/lexicon-iv.txt

wc -l $locdict/vocab-oov.txt
wc -l $locdict/lexicon-iv.txt

###  BEGIN SKIPPING GENERATING PRONUNCIACIONS FOR OOV WORDS ####
# pyver=`python --version 2>&1 | sed -e 's:.*\([2-3]\.[0-9]\+\).*:\1:g'`
# if [ ! -f tools/g2p/lib/python${pyver}/site-packages/g2p.py ]; then
#   echo "--- Downloading Sequitur G2P ..."
#   echo "NOTE: it assumes that you have Python, NumPy and SWIG installed on your system!"
#   wget -P tools http://www-i6.informatik.rwth-aachen.de/web/Software/g2p-r1668.tar.gz
#   tar xf tools/g2p-r1668.tar.gz -C tools
#   cd tools/g2p
#   echo '#include <cstdio>' >> Utility.hh # won't compile on my system w/o this "patch"
#   python setup.py install --prefix=.
#   cd ../..
#   if [ ! -f tools/g2p/lib/python${pyver}/site-packages/g2p.py ]; then
#     echo "Sequitur G2P is not found - installation failed?"
#     exit 1
#   fi
# fi
# 
# if [ ! -f conf/g2p_model ]; then
#   echo "--- Downloading a pre-trained Sequitur G2P model ..."
#   wget http://sourceforge.net/projects/kaldi/files/sequitur-model4 -O conf/g2p_model
#   if [ ! -f conf/g2p_model ]; then
#     echo "Failed to download the g2p model!"
#     exit 1
#   fi
# fi
# 
# echo "--- Preparing pronunciations for OOV words ..."
# python tools/g2p/lib/python${pyver}/site-packages/g2p.py \
#   --model=conf/g2p_model --apply $locdict/vocab-oov.txt > $locdict/lexicon-oov.txt

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

