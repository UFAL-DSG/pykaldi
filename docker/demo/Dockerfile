FROM      ufaldsg/pykaldi  
# ufaldsg/pykaldi is production image
MAINTAINER Ondrej Platek <ondrej.platek@gmail.com>
# Pykaldi is installed system wide we just need the dome scripts
RUN mkdir -p /app && cd /app && git clone https://github.com/UFAL-DSG/pykaldi.git
WORKDIR /app/pykaldi/online_demo
RUN echo "First lets download pretrained models and data" && make build_scp download_models
RUN echo "Running the standard Kaldi binaries in batch mode" && make gmm-latgen-faster
RUN echo "Running the incremental forward recognition with C++ wrapper" && make online-recogniser
RUN echo "Running the incremental forward recognition with Python wrapper" && make pyonline-recogniser
RUN echo "Having installed pykaldi natively on Ubuntu 14.04 you can check out live demo by running 'make live'"
