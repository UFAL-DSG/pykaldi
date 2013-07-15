
LIBNAME = dummyio
LIBFILE = lib$(LIBNAME).so
OBJFILES = dummyio.o frames.o debug.o
LDLIBS = -lasound
BIGLIB = libpykaldi.so

all: $(LIBFILE) $(BIGLIB)

CC = gcc
CXX = g++

CFLAGS = -msse -Wall -fPIC -Wno-sign-compare -g # -O0
# CXXFLAGS = $(CFLAGS)
LDFLAGS = -rdynamic

# extracts testing data
DECODE_DIR = work
include ../online-data.mk

$(LIBFILE): $(OBJFILES)
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(LDFLAGS) $(LDLIBS)

# Compiling the $(BIGLIB) in python-kaldi-decoding directory
$(BIGLIB):
	$(MAKE) -C ..  $@
	cp ../$@ .


kaldi_decoders.py: $(LIBFILE) $(BIGLIB)

kaldi_decoders_test.py:  kaldi_decoders.py $(LIBFILE) $(BIGLIB)


test: $(LIBFILE) $(BIGLIB) $(DATA_TAR)
	nosetests -s kaldi_decoders_test.py  # -s let the output be printed

clean:
	rm -rf $(OBJFILES) $(LIBFILE) $(BIGLIB) $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc test_frames.dat
