
LIBNAME = audio
LIBFILE = lib$(LIBNAME).so
OBJFILES = frames.o debug.o
# FIXME it is better to use portaudio;)
LDLIBS = -lasound

all: $(LIBFILE)

CC = gcc
CXX = g++

CFLAGS = -msse -Wall -fPIC -Wno-sign-compare -g # -O0
# CXXFLAGS = $(CFLAGS)
LDFLAGS = -rdynamic

$(LIBFILE): $(OBJFILES)
	$(CXX) -shared -o $@ -Wl,-soname=$@,--whole-archive $^ -Wl,--no-whole-archive $(LDFLAGS) $(LDLIBS)


test: $(LIBFILE)
	# use --no-skip if you have alsa installed (On Linux)
	# -s let the output be printed,
	nosetests -s audio_test.py 

clean:
	rm -rf $(OBJFILES) $(LIBFILE) *.pyc test_frames.dat
