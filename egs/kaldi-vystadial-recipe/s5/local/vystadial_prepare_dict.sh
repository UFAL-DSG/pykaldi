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
# _SIL_     ... we filter this already

renice 20 $$

locdata=data/local
locdict=$locdata/dict
local_arpa_lm=data/local/lm.arpa

mkdir -p $locdata

echo "=== Preparing the LM ..."

if [ -z "${ARPA_MODEL}" ]; then
    # prepare an ARPA LM and wordlist
    # KEEPING the OOV -> Allow train Kaldi for OOV model
    cp -f $ARPA_MODEL $local_arpa_lm
    echo "Using predefined LM in arpa format: ${ARPA_MODEL}"
else
    echo "=== Building LM of order ${lm_order}..."
    cut -d' ' -f2- data/train/text | sed -e 's:^:<s> :' -e 's:$: </s>:' | \
        grep -v '_INHALE_\|_LAUGH_\|_EHM_HMM_\|_NOISE_'  | \
        > $locdata/lm_train.txt
    # FIXME move it up after echo Building
    [ -z "$IRSTLM" ] && echo "Set IRSTLM env variable for building LM" && exit 1;
    build-lm.sh -i $locdata/lm_train.txt -n ${lm_order} -o $locdata/lm_phone_${lm_order}.ilm.gz

    compile-lm $locdata/lm_phone_${lm_order}.ilm.gz --text yes /dev/stdout | \
    grep -v unk | gzip -c > $local_arpa_lm 
fi

echo "=== Preparing the dictionary ..."

if [ -z "${DICTIONARY}" ]; then
    echo "Using predefined dictionary: ${DICTIONARY}"
fi

# # NOT ALLOWING OOV WORDS training & also in decoding
# grep -v -w OOV ${ARPA_MODEL} > data/local/lm.arpa 

# FIXME check that we have setup DICTIONARY correctly
echo '</s>' > data/local/vocab-full.txt
tail -n +3 $DICTIONARY | cut -d ' ' -f 1 |\
  sort | uniq >> data/local/vocab-full.txt 

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
echo "OOV SPN" > $locdict/lexicon-oov.txt
echo "_INHALE_ SPN" >> $locdict/lexicon-oov.txt
echo "_LAUGH_ SPN" >> $locdict/lexicon-oov.txt
echo "_EHM_HMM_ SPN" >> $locdict/lexicon-oov.txt
echo "_NOISE_ SPN" >> $locdict/lexicon-oov.txt

cat $locdict/lexicon-oov.txt $locdict/lexicon-iv.txt |\
  sort > $locdict/lexicon.txt

echo "--- Prepare phone lists ..."
echo SIL > $locdict/silence_phones.txt
echo _SIL_ >> $locdict/silence_phones.txt
echo SIL > $locdict/optional_silence.txt
grep -v -w sil $locdict/lexicon.txt | \
  awk '{for(n=2;n<=NF;n++) { p[$n]=1; }} END{for(x in p) {print x}}' |\
  sort > $locdict/nonsilence_phones.txt

echo "--- Adding SIL to the lexicon ..."
echo -e "!SIL\tSIL" >> $locdict/lexicon.txt

# Some downstream scripts expect this file exists, even if empty
touch $locdict/extra_questions.txt

echo "*** Dictionary preparation finished!"
