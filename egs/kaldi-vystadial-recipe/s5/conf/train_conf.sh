#!/bin/bash

data_lang=en
# Vystadial data 1 channel 16000 16-bit
# export DATA_ROOT="/ha/projects/vystadial/nobackup/VYSTADIAL-2013/data_voip_en"
export DATA_ROOT=/ha/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample/data

# data_lang=cs
# export DATA_ROOT=/ha/projects/vystadial/nobackup/VYSTADIAL-2013/data_voip_cs
# # export DATA_ROOT=/ha/work/people/oplatek/kaldi/egs/kaldi-vystadial-recipe/s5/Results/vystadial-sample-cs/data

export test_sets="dev test"

export LM_ORDER=2
# Unset or empty ARPA_MODEL variable means that the script will build the LM itself
# export ARPA_MODEL="/ha/projects/vystadial/git/alex/resources/lm/caminfo/arpa_trigram"
# unset ARPA_MODEL

# Should I create and use 0-gram LM for decoding from testing data?
export TEST_ZERO_GRAMS="yes"
# unset TEST_ZERO_GRAMS


# Unset or empty DICTIONARY variable means that the script will build the DICTIONARY itself
# export DICTIONARY="/ha/projects/vystadial/git/alex/resources/lm/caminfo/dict"
unset DICTIONARY

# Storage dir for MFCC. Need a lot of space.
export MFCC_DIR="./mfcc"

# Want to remove OOV from LM?
export NOOOV="yes"  # nonempty string triggers removing;)
# unset NOOOV


# How big portion of available data to use
# everyN=3    ->   we use one third of data
everyN=1

# Train monophone models on a subset of the data of this size
# monoTrainData=150
unset monoTrainData  # use full data

# Number of states for phonem training
pdf=1200

# Maximum number of Gaussians used for training
gauss=19200

# if run Cepstral Mean Normalisation: true/false
cmn=false

train_mmi_boost=0.05
