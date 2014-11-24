Pykaldi - Python Kaldi decoders wrapper
=======================================

Intro
-----
Pykaldi interfaces the C++ online recogniser which 
wraps the online feature preprocessing and online decoder 
from ``kaldi/src/onl-rec`` directory.
The development is done in https://github.com/UFAL-DSG/pykaldi.

Dependencies
------------
For Python dependencies see ``./pykaldi-requirements.txt`` and `PyFST <https://github.com/UFAL-DSG/pyfst>`_ 
For installation and other dependencies see ``INSTALL.rst`` in the root directory


Local Installation
------------------
Having the required dependencies installed, run ``make`` in this directory.

The ``Makefile`` cares for installing ``pyfst`` and ``pykaldi`` locally.

Note that the ``pyfst`` and ``pykaldi`` are local installations.
It enables multiple installation environments on one system.
The downside is that you need to setup ``PYTHONPATH`` and ``LD_LIBRARY_PATH`` variables.
All the scripts in ``kaldi/egs/vystadial/online_demo`` set up these variables.

Testing on sample data
----------------------
The demo in ``kaldi/egs/vystadial/online_demo`` directory wraps Pykaldi Python recognisers.
The scripts decode few sample utterances with pretrained acoustic and language models.
