Intro
-----
The official Kaldi toolkit installation is well documented at `installation guide <http://kaldi.sourceforge.net/install.html>`_.


KALDI on Ubuntu 12.04
~~~~~~~~~~~~~~~~~~~~~
In order to use Kaldi decoder, build ``pykaldi`` fork of Kaldi from https://github.com/UFAL-DSG/pykaldi,
install patched ``OpenFST`` from ``pykaldi``, then ``pyfst`` from https://github.com/UFAL-DSG/pyfst, and finally 
install ``pykaldi`` Python extension.

First,  build Kaldi fork ``pykaldi`` as follows:

.. code-block:: bash

  git clone https://github.com/UFAL-DSG/pykaldi
  cd pykaldi/tools
  make atlas   # Just downloads headers
  make openfst_tgt  # Install patched OpenFST LOCALLY!
  cd ../src
  ./configure  # Should find ATLAS libraries which you have installed via apptitude (easier way).
  make && make test
  cd dec-wrap && make && make test  # Directory needed for pykaldi Python wrapper

Install patched ``OpenFST`` system wide. The following commands install the already built ``OpenFST`` 
library from previous step:

.. code-block:: bash

    cd pykaldi/tools/openfst
    ./configure  --prefix=/usr  # Sets the path to system wide installation directory
    sudo make install  # Copies the already built and pathced libraries from 'make openfst_tgt' step.


Install ``pyfst`` by

.. code-block:: bash

    sudo pip install --upgrade pystache pyyaml cython
    
    git clone https://github.com/UFAL-DSG/pyfst.git pyfst
    cd pyfst
    sudo python setup.py install


Finally, install the ``pykaldi`` Python extension (a wrapper around Kaldi decoders):

.. code-block:: bash

    cd pykaldi/src/pykaldi
    sudo make install

Installing external dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

How have I installed OpenBlas?
------------------------------
.. code-block:: bash

    cd tools
    make openblas

How have I installed Openfst?
-----------------------------
.. code-block:: bash

    cd tools
    make openfst_tgt

How do I build Kaldi?
---------------------
.. code-block:: bash

    cd src
    ./configure  --openblas-root=`pwd`/../tools/OpenBLAS/install --fst-root=`pwd`/../tools/openfst --shared

If you updated from the git or svn repository, do not forget to run ``make depend``
Since by *default it is turned of! I always forget about that!*
.. code-block:: bash

    # make depend and make ext_depend are necessary only if dependencies changed
    make depend && make ext_depend && make && make ext 
    # optional test
    make test && make ext_test

How have I installed cython?
----------------------------
.. code-block:: bash

    pip install --user cython


How have I installed PortAudio?
-------------------------------
NOTE: Necessary only for Kaldi online decoder

.. code-block:: bash

    cd tools
    install_portaudio.sh


How did I update Kaldi source code?
-----------------------------------
I checked out the kaldi-trunk version.

`Kaldi install instructions <http://kaldi.sourceforge.net/install.html>`_

Note: If you checkout Kaldi before March 2013 you need to relocate svn. See the instructions in the link above!


What setup did I use?
---------------------
In order to use Kaldi binaries everywhere I add them to ``PATH``. 
In addition, I needed to add ``openfst`` directory to ``LD_LIBRARY_PATH``,
I compiled Kaldi dynamically linked against ``openfst``.
To conclude, I added following lines to my ``.bashrc``.
.. code-block:: bash

    ### Kaldi ###
    kaldisrc=/home/ondra/school/diplomka/kaldi/src
    export PATH="$PATH":$kaldisrc/bin:$kaldisrc/fgmmbin:$kaldisrc/gmmbin:$kaldisrc/nnetbin:$kaldisrc/sgmm2bin:$kaldisrc/tiedbin:$kaldisrc/featbin:$kaldisrc/fstbin:$kaldisrc/latbin:$kaldisrc/onlinebin:$kaldisrc/sgmmbin

    ### Openfst ###
    openfst=/ha/home/oplatek/50GBmax/kaldi/tools/openfst
    export PATH="$PATH":$openfst/bin
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH":$openfst/lib 

How have I installed Atlas?
---------------------------
 * NOTE1: On Ubuntu 12.04 for Travis CI I used Debian packages. See [travis.yml](./.travis.yml)
            and I just download ATLAS headers by ``cd tools; make atlas``.
 * NOTE2: There is prepared installation script ``tools/install_atlas.sh`` which you should try first. 
          If it fails, you may find the help in this section.

How I install Atlas:
 * I installed version atlas3.10.1.tar.bz2 (available at sourceforge)
 * I unpackaged it under ``tools`` which created ``tools/ATLAS``
 * The crucial problem with building ATLAS was disabling CPU throtling. I solved it by:

.. code-block:: bash

    # running following command under root in my Ubuntu 12.10
    # It does not turn off CPU throttling in fact, but I do not need the things optimaze on my local machine
    # I ran it for all of my 4 cores
    # for n in 0 1 2 3 ; do echo 'performance' > /sys/devices/system/cpu/cpu${n}/cpufreq/scaling_governor ; done

 * I needed to install Fortran compiler (The error from configure was little bit covered by consequent errors)

.. code-block:: bash

    sudo apt-get install gfortran

 * On Ubuntu 12.04 I had issue with 

.. code-block:: bash

    /usr/include/features.h:323:26: fatal error: bits/predefs.h

   Which I solved by

.. code-block:: bash

    sudo apt-get install --reinstall libc6-dev

 * Finally, in ``tools/ATLAS``I run:

.. code-block:: bash

    mkdir build 
    mkdir ../atlas_install
    cd build
    ../configure --shared --incdir=`pwd`/../../atlas_install
    make 
    make install
