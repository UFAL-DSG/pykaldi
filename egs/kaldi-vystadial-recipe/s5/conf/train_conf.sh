#!/bin/bash

# Vystadial data: 1 channel, 16000Hz, 16-bit wav files
export DATA_ROOT="/ha/projects/vystadial/data/asr/en/voip/"

# How big portion of available data to use
# everyN=3    ->   we use one third of data
everyN=1


export LM_ORDER=3 

# Unset or empty ARPA_MODEL variable means 
# that the script will build the LM itself
# unset ARPA_MODEL 

# The script just copies the arpa LM 
# export ARPA_MODEL="/ha/projects/vystadial/git/alex/resources/lm/caminfo/arpa_trigram"


# Unset or empty DICTIONARY variable means 
# that the script will build the DICTIONARY itself
# unset DICTIONARY

export DICTIONARY="/ha/projects/vystadial/git/alex/resources/lm/caminfo/dict"


# Storage dir for MFCC. Need a lot of space. 
export MFCC_DIR="./mfcc"


# Train monophone models on a subset of the data of this size
monoTrainData=1000

# Number of states for phonem training
pdf=1200

# Maximum number of Gaussians used for training
gauss=19200


train_mmi_boost=0.05

