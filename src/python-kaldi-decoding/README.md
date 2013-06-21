Intro
-----
The goal of this project is to test
Kaldi decoding pipeline called from Python using [cffi](http://cffi.readthedocs.org/en/release-0.6/).

Prerequisities
--------------
UPDATE 2013 06 20
Since I committed changes to `svn.code.sf.net/p/kaldi/sandbox/oplatek` you can easily setup `OpenBLAS` instead of `ATLAS` and compile `OpenFst`, `PortAudio` and `Kaldi` itself much easier with *shared library support*.
 *Shared library support* is needed for `cffi`. 
 
I hope the sandbox will be merged soon into kaldi/trunk.

Apart the Kaldi stuff (OpenFST, OpenBLAS, PortAudio) you need to install obviously `cffi`.
In the `sanbox/oplatek/tools/extras/` you can find `install_cffi.sh`.

I recommend to install `cffi` via you system instalater and use it for other stuff too,
but you can install it by `install_cffi.sh` into `tools` directory.


Running and building examples
-----------------------------

In order to build shared libraries and run C test binaries
```sh
$make all
```
To run `run_online.py`, please, specify where are the shared libraries. E.g. by running from `kaldi-trunk/src/python-kaldi-decoding`.
```sh
LD_LIBRARY_PATH=`pwd`/../../tools/OpenBLAS:`pwd`/../../tools/openfst/lib:`pwd` ./run_online.py
```
For details and running other commands run tests and check the `Makefile`
```sh
make test
```



Remarks on linking
-------
 * [How to use dlopen](http://www.isotton.com/devel/docs/C++-dlopen-mini-HOWTO/C++-dlopen-mini-HOWTO.html)
 * [Stackoverflow little off topic explanation](http://stackoverflow.com/questions/12762910/c-undefined-symbols-when-loading-shared-library-with-dlopen)
 * [http://kaldi.sourceforge.net/matrixwrap.html](See Missing the ATLAS implementation of  CLAPACK)
 * I spent a lot of time to set right linking. 
    I was linking `lapack` libraries instead of `lapack_atlas`.
    I was getting error `undefined symbol: clapack_dgetrf`
