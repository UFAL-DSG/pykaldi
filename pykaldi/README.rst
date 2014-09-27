Pykaldi - Python Kaldi decoders wrapper
=======================================

Intro
-----
Pykaldi interfaces the C++ online recogniser which 
wraps the online feature preprocessing and online decoder 
from ``kaldi/src/onl-rec`` directory.
The development is done in https://github.com/UFAL-DSG/pykaldi.
Currently, we do not support CMN, so train your AM without CMN.

Dependencies
------------
* OpenFST - You should use the installation build via ``cd kaldi/src/tools; make openfst_tgt``
* Cython 19.1+  ``pip install cython>=19.1``
* Our fork of `PyFST <https://github.com/UFAL-DSG/pyfst>`_ which transitively requires ``pyyaml``, ``pystache``
  - Not necessary to install. Local ``Makefile`` will install it for you.
* For Python 2.6 also install ``pip install argparse unittest2 ordereddict``.


Local Installation
------------------
Having the required dependencies installed, run ``make`` in this directory.

The ``Makefile`` cares for installing ``pyfst`` and ``pykaldi`` locally.

Note that the ``pyfst`` and ``pykaldi`` are local installations.
It enables multiple installation environments on one system.
The downside is that you need to setup ``PYTHONPATH`` and ``LD_LIBRARY_PATH`` variables.
All the scripts in ``kaldi/egs/vystadial/online_demo`` set up these variables.
Please use the scripts as a reference or 
install the packages system wide via ``python setup.py install``.
The system wide installation is descriped in ``INSTALL.rst`` 
in the root directory of this git repository.

Testing on sample data
----------------------
The demo in ``kaldi/egs/vystadial/online_demo`` directory wraps Pykaldi Python recognisers.
The scripts decode few sample utterances with pretrained acoustic and language models.
