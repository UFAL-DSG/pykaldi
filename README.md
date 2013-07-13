Description
=====
 * The main goal is to create a Python wrapper suitable for real-time decoding in a dialog system
 * The wrapper is built on recent [Kaldi](http://sourceforge.net/projects/kaldi/)
 * The [Svn trunk of Kaldi project](svn://svn.code.sf.net/p/kaldi/code/trunk) is mirrored to branch `svn_mirror`.
 * The `master` branch is based on `svn_mirror` and in the `master` the Python wrapper is developed. 
 * I am using [cffi](http://cffi.readthedocs.org/en/latest/) for interfacing C from Python.
 * I also trained several acoustic models.  
 * The recipe for training the models can be found at `egs/kaldi-vystadial-recipe`. Read `egs/kaldi-vystadial-recipe/s5/README.md`!
 * The python wrapper is at `src/python-kaldi-decoding`. Read `src/python-kaldi-decoding/README.md`!


Install
-------
 * Read `INSTALL.md` and `INSTALL` first!
 * `INSTALL.md` constains

Other info
----------
 * This work is done under [Vystadial project](https://sites.google.com/site/filipjurcicek/projects/vystadial).
 * The svn trunk is mirrored via `git svn`. [Nice intro to git svn](http://viget.com/extend/effectively-using-git-with-subversion), [Walk through](http://blog.shinetech.com/2009/02/17/my-git-svn-workflow/) and [Multiple svn-remotes](http://blog.shuningbian.net/2011/05/git-with-multiple-svn-remotes.html)
 * I would like to merge as many changes as possible back to Kaldi and I already have helped on changes which were merged back to Kaldi.

LICENSE
--------
 * We release all the changes at Pykaldi under `Apache license 2.0` license. Kaldi also uses `Apache 2.0` license. 
 * We also want to publicly release the training data in the autumn 2013.
