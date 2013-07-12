
all:

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
$(BIGLIB):
	$(MAKE) -C ../..  $@
	cp ../../$@ .


kaldi_decoders.py: $(LIBFILE) $(BIGLIB)

kaldi_decoders_test.py:  kaldi_decoders.py $(LIBFILE) $(BIGLIB)



test: $(LIBFILE) $(BIGLIB)
	nosetests -s kaldi_decoders_test.py  # -s let the output be printed

clean:
	rm -f $(OBJFILES) $(LIBFILE) $(BIGLIB) *.pyc test_frames.dat
