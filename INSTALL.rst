Intro
-----
The official Kaldi toolkit installation is well documented at `installation guide <http://kaldi.sourceforge.net/install.html>`_.


Installation
~~~~~~~~~~~~~~~~~~~~~
In order to use Kaldi decoder system wide
follow the instructions in ``Dockerfile`` or ``.travis.yml`` (For Ubuntu 14.04 or 12.04),
and update the build instructions for your system.

This fork of Kaldi is developed in master branch at https://github.com/UFAL-DSG/pykaldi.
We use ``pyfst`` from https://github.com/UFAL-DSG/pyfst, which is installed on top of patched version of OpenFST
