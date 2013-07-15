Description
=====
 * The main goal is to create a Python wrapper suitable for real-time decoding in dialog systems.
 * The wrapper is built on recent [Kaldi](http://sourceforge.net/projects/kaldi/) version mirrored from Kaldi Svn trunk.
 * The [Svn trunk of Kaldi project](svn://svn.code.sf.net/p/kaldi/code/trunk) is mirrored to branch `svn_mirror`.
 * The Python wrapper is developed in the `master` branch, which is based on `svn_mirror` branch. 
 * I am using [cffi](http://cffi.readthedocs.org/en/latest/) for interfacing C from Python.
 * I also trained several acoustic models.  
 * The recipe for training the models can be found at `egs/kaldi-vystadial-recipe`. Read `egs/kaldi-vystadial-recipe/s5/README.md`!
 * The python wrapper is at `src/pykaldi`. Read `src/pykaldi/README.md`!


Install
-------
 * Read `INSTALL.md` and `INSTALL` first!
 * `INSTALL.md` contains instructions specific for Pykaldi. `INSTALL` stores general instructions for Kaldi.

Other info
----------
 * Pykaldi is developed under [Vystadial project](https://sites.google.com/site/filipjurcicek/projects/vystadial).
 * The svn trunk is mirrored via `git svn`. Checkout tutorials: [Nice intro to git svn](http://viget.com/extend/effectively-using-git-with-subversion), [Walk through](http://blog.shinetech.com/2009/02/17/my-git-svn-workflow/) and [Multiple svn-remotes](http://blog.shuningbian.net/2011/05/git-with-multiple-svn-remotes.html)
 * I would like to merge as many changes as possible back to Kaldi. (I already have helped on changes which were merged back to Kaldi.)

LICENSE
--------
 * Pykaldi is released under the [Apache license, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0). Kaldi also uses `Apache 2.0` license. 
 * We also want to publicly release the training data in the autumn 2013.
