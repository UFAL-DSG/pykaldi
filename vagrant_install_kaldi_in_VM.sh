#!/bin/bash

set -e # Stop on any error

sudo apt-get install build-essential libatlas-base-dev

pushd tools
  make atlas openfst_tgt
popd

pushd src
./configure --shared && \
make depend && \
make && \
echo 'KALDI LIBRARY INSTALLED OK'

pushd onl-rec && \
make depend && \
make && \
make test && \
echo 'OnlineLatgenRecogniser build OK'
popd # onl-rec

pushd pykaldi
make && echo 'INSTALLATION Works OK'
popd # pykaldi

popd # src

pushd egs/vystadial/online_demo/

echo; echo 'Downloading data and running default Kaldi batch decoder on 1 utterance to verify installation' ; echo
echo; echo 'You are now in online_demo directory for (Py)OnlineLatgenRecogniser demo'; echo

make gmm-latgen-faster

echo; echo 'Run $make pyonline-recogniser   to see the recogniser on sample prerecorded data'; echo


echo; echo 'Run $make live   to launch a localhost webserver'; echo
