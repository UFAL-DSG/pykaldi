Intro
-----
Kaldi has very good  [installation guide](http://kaldi.sourceforge.net/install.html). It is easy and straightforward.

Installing external dependencies
================================

How have I installed OpenBlas?
----------------------
Simple enough:
```bash
cd tools
make openblas
```

How have I installed Openfst?
----------------------
```bash
cd tools
make openfst_tgt
```

How do I build Kaldi?
------------------
```bash
cd src
./configure  --openblas-root=`pwd`/../tools/OpenBLAS/install --fst-root=`pwd`/../tools/openfst --shared
```

If you updated from the git or svn repository, do not forget to run `make depend`
Since by *default it is turned of! I always forget about that!*

```
# make depend and make ext_depend are necessary only if dependencies changed
make depend && make ext_depend && make && make ext 
# optional test
make test && make ext_test
```

How do I install [cffi](http://cffi.readthedocs.org/en/latest/)?
----------------------------------------------------------------
```bash
pip install --user cffi
```
If it does not work for you the script `tools/install_cffi.sh` should help you.
```bash
cd tools
./install_cffi.sh
```


How have I installed PortAudio?
--------------------------
NOTE: Necessary only for Kaldi online decoder

```bash
cd tools
install_portaudio.sh
```


How did I update Kaldi source code?
----------------------------
I checked out the kaldi-trunk version.

[Kaldi install instructions](http://kaldi.sourceforge.net/install.html)

Note: If you checkout Kaldi before March 2013 you need to relocate svn. See the instructions in the link above!


What setup did I use?
--------------------
In order to use Kaldi binaries everywhere I add them to `PATH`. 
In addition, I needed to add `openfst` directory to `LD_LIBRARY_PATH`, I compiled Kaldi dynamically linked against `openfst`. To conclude, I added following lines to my `.bashrc`.
```bash
### Kaldi ###
kaldisrc=/home/ondra/school/diplomka/kaldi/src
export PATH="$PATH":$kaldisrc/bin:$kaldisrc/fgmmbin:$kaldisrc/gmmbin:$kaldisrc/nnetbin:$kaldisrc/sgmm2bin:$kaldisrc/tiedbin:$kaldisrc/featbin:$kaldisrc/fstbin:$kaldisrc/latbin:$kaldisrc/onlinebin:$kaldisrc/sgmmbin

### Openfst ###
openfst=/ha/home/oplatek/50GBmax/kaldi/tools/openfst
export PATH="$PATH":$openfst/bin
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH":$openfst/lib 
```

How have I installed Atlas?
--------------------
 * NOTE1: I normally use OpenBLAS instead of ATLAS. 
 * NOTE2: There is prepared installation script `tools/install_atlas.sh` which you should try first. 
          If it fails, you may find the help in this section.
 * NOTE3: On Ubuntu 12.04 for Travis CI I used Debian packages. See [travis.yml](./.travis.yml).

How I install Atlas:
 * I installed version atlas3.10.1.tar.bz2 (available at sourceforge)
 * I unpackaged it under `tools` which created `tools/ATLAS`
 * The crucial problem with building ATLAS was disabling CPU throtling. I solved it by:

```bash
# running following command under root in my Ubuntu 12.10
# It does not turn off CPU throttling in fact, but I do not need the things optimaze on my local machine
# I ran it for all of my 4 cores
# for n in 0 1 2 3 ; do echo 'performance' > /sys/devices/system/cpu/cpu${n}/cpufreq/scaling_governor ; done
```

 * I needed to install Fortran compiler (The error from configure was little bit covered by consequent errors)

```bash
sudo apt-get install gfortran
```

 * On Ubuntu 12.04 I had issue with 

```bash
/usr/include/features.h:323:26: fatal error: bits/predefs.h
```

   Which I solved by

```bash
sudo apt-get install --reinstall libc6-dev
```

 * Finally, in `tools/ATLAS` I run:

```bash
mkdir build 
mkdir ../atlas_install
cd build
../configure --shared --incdir=`pwd`/../../atlas_install
make 
make install
 ```
