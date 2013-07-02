
all:

LIBNAME = dummyio
LIBFILE = lib$(LIBNAME).so
OBJFILES = dummyio.o
LDLIBS =

CC = g++
CXX = g++

CXXFLAGS = -msse -Wall -fPIC -Wno-sign-compare -g # -O0
LDFLAGS = -rdynamic


all: $(LIBFILE)


$(LIBFILE): $(OBJFILES)
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(LDFLAGS) $(LDLIBS)

kaldi_decoders.py: $(LIBFILE)

kaldi_decoders_test.py:  kaldi_decoders.py $(LIBFILE)

test: $(LIBFILE)
	nosetests kaldi_decoders_test.py

clean:
	rm -f $(OBJFILES) $(LIBFILE) *.pyc test_frames.dat
