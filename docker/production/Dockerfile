FROM      ubuntu:14.04
MAINTAINER Ondrej Platek <ondrej.platek@gmail.com>

RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
RUN apt-get update && apt-get install -y build-essential libatlas-base-dev python-dev python-pip git wget zip

RUN mkdir -p /app && cd /app && git clone --recursive https://github.com/UFAL-DSG/pykaldi.git
WORKDIR /app/pykaldi

RUN pip install -r pykaldi/pykaldi-requirements.txt

# Installing kaldi
RUN make && make test && make install && ldconfig && cd / && python -c 'import kaldi.decoders' && echo 'Pykaldi library installation succesfull!'
