FROM      ubuntu:14.04
MAINTAINER Ondrej Platek <ondrej.platek haha gmail.com>


ADD . /app/pykaldi
WORKDIR /app/pykaldi
RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
RUN apt-get update && apt-get install -y build-essential libatlas3-base libatlas-base-dev python-dev python-pip git wget gfortran
RUN pip install -r pykaldi/pykaldi-requirements.txt

WORKDIR  /app/pykaldi/pykaldi
RUN make
RUN make test
