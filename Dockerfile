FROM      ubuntu:14.04
MAINTAINER Ondrej Platek <ondrej.platek haha gmail.com>


ADD . /app/pykaldi
WORKDIR /app/pykaldi
RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
RUN apt-get update && apt-get install -y build-essential libatlas-base-dev python-dev python-pip git wget zip
RUN pip install -r pykaldi/pykaldi-requirements.txt

# testing the installation
RUN make distclean
RUN make
RUN make test

# installing kaldi
RUN make install
RUN ldconfig
# final testing
WORKDIR /
RUN python -c 'import kaldi.decoders'
