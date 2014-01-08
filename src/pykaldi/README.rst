Pykaldi - Python Kaldi decoders wrapper
=======================================

Dependencies
------------
* OpenFST - You should use the installation build via ``cd kaldi/src/tools; make openfst_tgt``
* Cython 19.1+  ``pip install cython>=19.1``
* Our fork of `PyFST <https://github.com/UFAL-DSG/pyfst>`_ which transitively requires ``pyyaml``, ``pystache``
  - Not necessary to install. Local ``Makefile`` will install it for you.


Installation
------------
Having the required dependencies installed, run ``make`` in this directory.

The ``Makefile`` cares for installing ``pyfst`` and ``pykaldi`` locally.

Note that the ``pyfst`` and ``pykaldi`` are local installations.
It enables multiple installation environments on one system.
The downside is that you need to setup ``PYTHONPATH`` and ``LD_LIBRARY_PATH`` variables.
All the scripts in `./pykaldi/pykaldi/binutils` set up these variables.
Please use the scripts as a reference or 
install the packages system wide via ``python setup.py install``.

Testing on sample data
----------------------
In the ``pykaldi/binutils`` directory there are scripts,
which wraps Kaldi Python decoders.
The scripts decode few sample utterances with 
pretrained toy acoustic and language models.
The scripts are described in `.pykaldi/binutils/README.rst`

