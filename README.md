ABOUT
=====
 * This is a Git mirror of [Svn trunk of Kaldi project](http://sourceforge.net/projects/kaldi/)
   `svn://svn.code.sf.net/p/kaldi/code/trunk`
 * In the branch `master` I commit my work. In the branch `svn_mirror` I mirror `svn://svn.code.sf.net/p/kaldi/code/trunk`.
 * Currently, I mirror the repository manually as often as I needed.
 * I am using [cffi](http://cffi.readthedocs.org/en/latest/) for interfacing C from Python
 * The main purpose for mirroring is that I want to build my own decoder and train my models for decoding based on up-to-date Kaldi version.
 * Recipe for training the models can be found at `egs/kaldi-vystadial-recipe`
 * Source code for python wrapper for decoders is at `src/python-kaldi-decoding` 
 * I mirror the svn via `git svn`. [Nice intro to git svn](http://viget.com/extend/effectively-using-git-with-subversion), [Walk through](http://blog.shinetech.com/2009/02/17/my-git-svn-workflow/) and [Multiple svn-remotes](http://blog.shuningbian.net/2011/05/git-with-multiple-svn-remotes.html)

OTHER INFO
----------
 * Read `INSTALL.md` and `INSTALL` first!
 * For training models read `egs/kaldi-vystadial-recipe/s5/README.md`
 * For building and developing decoder callable from Python read `src/python-kaldi-decoding/README.md`
 * This work is done under [Vystadial project](https://sites.google.com/site/filipjurcicek/projects/vystadial).

LICENSE
--------
 * We release all the changes at pyKaldi under `Apache license 2.0` license. Kaldi also uses `Apache 2.0` license. 
 * We also want to publicly release the training data in the autumn 2013.
