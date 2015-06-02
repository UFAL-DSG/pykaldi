FROM      ufaldsg/pykaldi  
# ufaldsg/pykaldi is production image
MAINTAINER Ondrej Platek <ondrej.platek@gmail.com>
# Pykaldi is installed system wide but still need the demo scripts
WORKDIR /app/pykaldi/online_demo
RUN echo "First download pretrained models and test data." && echo && make build_scp download_models
CMD echo "Running the standard Kaldi binaries in batch mode." && make gmm-latgen-faster && \
 echo "Running the incremental forward recognition with C++ wrapper." && make online-recogniser && \
 echo "Running the incremental forward recognition with Python wrapper." && make pyonline-recogniser && \
 echo "Having installed pykaldi natively on Ubuntu 14.04 you can check out live demo by running 'make live'."
