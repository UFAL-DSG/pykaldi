Intro
-----
The goal of this project is to interface Kaldi decoders from Python. 
Subgoals:
 * Build shared libraries for Kaldi
 * Python/C interface for Kaldi using [cffi](http://cffi.readthedocs.org/en/release-0.6/).
 * _Online_ interface to decoders which can be fed with audio data frame by frame. 
 * Make the _Online_ iterface available also from Python

Content
-------
 * C and C++ files for testing shared libraries compliation
 * `pykaldi` directory containing C/Python interface to Kaldi

Prerequisites
--------------
UPDATE 2013 07 02
`Kaldi/sandbox/sharedlibs` now support compiling Kaldi with shared libraries.
 *Shared library support* is needed for `cffi`. 
 
I hope the sandbox will be merged soon into kaldi/trunk.

Apart the Kaldi stuff (OpenFST, OpenBLAS, PortAudio) you need to install obviously `cffi`.
In the `sanbox/oplatek/tools/extras/` you can find `install_cffi.sh` installation script.

I recommend to install `cffi` via you system package manager and use it for other stuff too.
However, you can always install it by running `install_cffi.sh` from `tools` directory.


Running and building examples
-----------------------------

[DEPRECATED] todo rewrite it with python setup.py develop/install/test
In order to build shared libraries and run C test binaries run following commands from this directory.

python setup.py develop --user vs  python setup.py develop --uninstall
python setup.py install --user vs  pip uninstall pykaldi

Note, that install fails due to missing headers: It is reasonable!
There are no headers for Kaldi in system paths! So only develop mode is available!
When Kaldi is ready to be installed in system path it will be easy to install it too!



Remarks on linking
-------
 * [How to use dlopen](http://www.isotton.com/devel/docs/C++-dlopen-mini-HOWTO/C++-dlopen-mini-HOWTO.html)
 * [Stackoverflow little off topic explanation](http://stackoverflow.com/questions/12762910/c-undefined-symbols-when-loading-shared-library-with-dlopen)
 * [http://kaldi.sourceforge.net/matrixwrap.html](See Missing the ATLAS implementation of  CLAPACK)
 * I spent a lot of time to set right linking. 
    I was linking `lapack` libraries instead of `lapack_atlas`.
    I was getting error `undefined symbol: clapack_dgetrf`
