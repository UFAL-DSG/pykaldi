#!/bin/bash

# Vystadial data 1 channel 16000 16-bit
export DATA_ROOT="/ha/projects/vystadial/data/asr/en/voip/"

# Test-time language model order
# We are just copying the arpa LM (3-order)
lm_order=3 
export ARPA_MODEL="/ha/projects/vystadial/git/alex/resources/lm/caminfo/arpa_trigram"

export DICTIONARY="/ha/projects/vystadial/git/alex/resources/lm/caminfo/dict"

# Storage dir for MFCC. Need a lot of space. 
export MFCC_DIR="./mfcc"


# How big portion of available data to use
# everyN=3    ->   we use one third of data
everyN=1

# Train monophone models on a subset of the data of this size
monoTrainData=1000

# Number of states for phonem training
pdf=1200

# Maximum number of Gaussians used for training
gauss=19200


train_mmi_boost=0.05

