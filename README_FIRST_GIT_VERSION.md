ABOUT
=====
 * This is a Git mirror of [Svn trunk of Kaldi project](http://sourceforge.net/projects/kaldi/)
   `svn checkout svn://svn.code.sf.net/p/kaldi/code/trunk`
 * Currently, mirroring is done by hand. It is based on my needs for up-to-date changes in Kaldi.
 * The main purpose for mirroring is that I want to build my own decoder and train my models for decoding
 * The other "features" are located at `egs/kaldi-vystadial-recipe`, `src/python-kaldi-decoding` and at `src/vystadial-decoder`
 * I am using the `Fake submodules` approach described at [this blog](http://debuggable.com/posts/git-fake-submodules:4b563ee4-f3cc-4061-967e-0e48cbdd56cb)

LILCENSE
--------
 * One of our goals is to release all of the added features under `Apache license 2.0` (Kaldi uses the same license). However, we did not added any license statement to repository yet. If you see this README and do not know the licensing conditions, write us. We probably just forget to put the license under the repository.
 * We also want to publicly release the training data
