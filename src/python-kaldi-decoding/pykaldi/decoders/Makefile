
# it adds only one target: DATA_TAR for obtaining test data
include ../online-data.mk
# params for online-data.mk
DECODE_DIR = work
INPUT_SCP = $(DECODE_DIR)/input.scp
SPK2UTT = $(DECODE_DIR)/spk2utt

LIBNAME = dummyio
LIBFILE = lib$(LIBNAME).so
OBJFILES = dummyio.o frames.o debug.o
LDLIBS = -lasound
BIGLIB = libpykaldi.so

CC = gcc
CXX = g++

CFLAGS = -msse -Wall -fPIC -Wno-sign-compare -g # -O0
# CXXFLAGS = $(CFLAGS)
LDFLAGS = -rdynamic


all: $(LIBFILE) $(BIGLIB)


$(LIBFILE): $(OBJFILES)
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(LDFLAGS) $(LDLIBS)

# Compiling the $(BIGLIB) in python-kaldi-decoding directory
$(BIGLIB): ../../$(BIGLIB)
	$(MAKE) -C ../..  $@
	cp ../../$@ .


kaldi_decoders.py: $(LIBFILE) $(BIGLIB)

kaldi_decoders_test.py:  kaldi_decoders.py $(LIBFILE) $(BIGLIB) $(DATA_TAR)


test: $(LIBFILE) $(BIGLIB)
	nosetests -s kaldi_decoders_test.py  # -s let the output be printed

clean:
	rm -rf $(OBJFILES) $(LIBFILE) $(BIGLIB) $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc test_frames.dat
