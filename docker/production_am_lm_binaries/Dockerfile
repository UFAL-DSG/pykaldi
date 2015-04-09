FROM      ufaldsg/pykaldi
MAINTAINER Ondrej Platek <ondrej.platek@gmail.com>

ENTRYPOINT ["/bin/bash"]

WORKDIR /app/pykaldi
# install kaldi binaries and test it
RUN apt-get update && apt-get install -y subversion cmake zlib1g-dev
RUN make install-kaldi-binaries && apply-cmvn && echo 'Kaldi binaries installed succesfully!'
RUN make install-irstlm
