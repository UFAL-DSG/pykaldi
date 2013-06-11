ABOUT
=====
 * This is a Git mirror of [Svn trunk of Kaldi project](http://sourceforge.net/projects/kaldi/)
   `svn://svn.code.sf.net/p/kaldi/code/trunk`
 * Currently, I mirror the repository manually as often as I needed.
 * The main purpose for mirroring is that I want to build my own decoder and train my models for decoding based on up-to-date Kaldi version.
 * Recipe for training the models can be found at `egs/kaldi-vystadial-recipe`
 * Source code for python wrapper for online-decoder is at `src/python-kaldi-decoding` 
 * Remarks about new decoder are located at `src/vystadial-decoder`
 * I use the `Fake submodules` approach to merge 3 subprojects to this repository. More about `Fake submodules` [at this blog](http://debuggable.com/posts/git-fake-submodules:4b563ee4-f3cc-4061-967e-0e48cbdd56cb).
 * I mirror the svn via `git svn`. [Nice intro to git svn](http://viget.com/extend/effectively-using-git-with-subversion)

LICENSE
--------
 * One of our goals is to release all of the added features under `Apache license 2.0` (Kaldi uses the same license). However, we did not added any license statement to repository yet. If you see this README and do not know the licensing conditions, write us. We probably just forget to put the license under the repository.
 * We also want to publicly release the training data
