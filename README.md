Description
=====
 * Goal: Create a Python wrapper suitable for real-time decoding in a dialog systems.
 * Based on: the [Svn trunk of Kaldi project](svn://svn.code.sf.net/p/kaldi/code/trunk) which is mirrored to branch `svn-mirror`.
 * The [cffi](http://cffi.readthedocs.org/en/latest/) library is used for interfacing C from Python.
 * There is also a new Kaldi recipe for training acoustic models. See [egs/kaldi-vystadial-recipe](egs/kaldi-vystadial-recipe) and [README.md](egs/kaldi-vystadial-recipe/README.md)!
 * The python wrapper is at [src/pykaldi](src/pykaldi). Read the [README.md](./src/pykaldi/README.md)!


Install
-------
[![Build Status](https://travis-ci.org/oplatek/pykaldi.png)](https://travis-ci.org/oplatek/pykaldi)
 * Read [INSTALL.md](./INSTALL.md) and [INSTALL](./INSTALL) first!
 * [INSTALL.md](./INSTALL.md) contains instructions specific for Pykaldi. [INSTALL](./INSTALL) stores general instructions for Kaldi.


Other info
----------
 * Pykaldi is developed under [Vystadial project](https://sites.google.com/site/filipjurcicek/projects/vystadial).
 * The svn trunk is mirrored via `git svn`. Checkout tutorials: [Intro to git svn](http://viget.com/extend/effectively-using-git-with-subversion), 
 [Git svn branch in git repo](http://ivanz.com/2009/01/15/selective-import-of-svn-branches-into-a-gitgit-svn-repository/)

LICENSE
--------
 * Pykaldi is released under the [Apache license, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0). Kaldi also uses `Apache 2.0` license. 
 * We also want to publicly release the training data in the autumn 2013.
