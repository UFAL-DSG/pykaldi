FROM      ubuntu:14.04
MAINTAINER Ondrej Platek <oplatek@ufal.mff.cuni.cz>

#
# Install PyKaldi.
#

# Prerequesities.
RUN apt-get update
RUN apt-get install -y build-essential libatlas-base-dev python-dev python-pip git wget
# RUN apt-get libportaudio-dev portaudio19-dev libsox-dev  # not necessary

WORKDIR /app
RUN git clone https://github.com/UFAL-DSG/pykaldi/
WORKDIR /app/pykaldi

# PyKaldi tools.
WORKDIR tools
RUN make atlas openfst_tgt
RUN ldconfig

# Compile the Kaldi src.
WORKDIR ../src
RUN ./configure --shared && make depend && make && echo 'KALDI LIBRARY INSTALLED OK'

# Compile Online recogniser.
WORKDIR onl-rec
RUN make && make test && echo 'OnlineLatgenRecogniser build OK'

# Compile Kaldi module for Python.
WORKDIR ../pykaldi
RUN pip install -r pykaldi-requirements.txt
RUN make && echo 'Pykaldi BUILD Works OK'
RUN make install && echo 'Pykaldi INSTALL Works OK'
RUN ldconfig

# Misc.
# WORKDIR /app/pykaldi/egs/vystadial/online_demo/
# RUN make gmm-latgen-faster
