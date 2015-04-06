FROM      ubuntu:14.04
MAINTAINER Ondrej Platek <ondrej.platek haha gmail.com>


RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
RUN apt-get update && apt-get install -y build-essential libatlas-base-dev python-dev python-pip git wget zip

ADD pykaldi/pykaldi-requirements.txt /tmp/pykaldi-requirements.txt
RUN pip install -r /tmp/pykaldi-requirements.txt
RUN mkdir -p /app/pykaldi
WORKDIR /app/pykaldi
RUN echo 'Pykaldi dependencies installed. Use docker run with -v flag e.g. pykaldi/docker/dev to develop pykaldi'

