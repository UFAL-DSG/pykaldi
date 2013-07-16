#!/bin/bash
# The installation scripts worked for Ubuntu 10.04 and is based on TOOLS/INSTALL
# instructions for installing IRST LM 

NAME=irstlm-5.80.03

wget http://sourceforge.net/projects/irstlm/files/irstlm/irstlm-5.80/${NAME}.tgz

tar xfv ${NAME}.tgz

ln -s $NAME irstlm

cd irstlm 

./regenerate-makefiles.sh  
./regenerate-makefiles.sh  # run this twice; it seems to be necessary.

./configure --prefix=`pwd` && make && make install
