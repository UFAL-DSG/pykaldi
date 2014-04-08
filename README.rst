Fork of Kaldi for developing online-lattice recogniser
======================================================

Summary
-------
The fork presents three new Kaldi features:

* Online Lattice Recogniser. The best results were obtained using MFCC, LDA+MLLT, bMMI acoustic models and feature transformations.
* Python wrapper which interfaces the ``OnlineLatticeRecogniser`` to Python.
* Training scripts which can be used with standard Kaldi tools or with the new ``OnlineLatticeRecogniser``.
  The scripts for Czech and English support acoustic models obtained using MFCC, LDA+MLLT/delta+delta-delta feature transformations and acoustic models trained generatively or by MPE or bMMI training.

The new functionality is separated to different directories:
 * kaldi/src/onl-rec stores C++ code for ``OnlineLatticeRecogniser``.
 * kaldi/scr/pykaldi stores Python wrapper ``PyOnlineLatticeRecogniser``.
 * kaldi/egs/vystadial/s5 stores training scripts.
 * kaldi/egs/vystadial/online_demo shows Kaldi standard decoder, ``OnlineLatticeRecogniser`` and ``PyOnlineLatticeRecogniser``, which produce the exact same lattices using the same setup.

The ``OnlineLatticeRecogniser`` is used in Alex dialogue system (https://github.com/UFAL-DSG/alex).

In March 2014, the ``PyOnlineLatticeRecogniser`` recogniser was evaluated on Alex domain.
See graphs evaluating ``OnlineLatticeRecogniser`` performance at http://nbviewer.ipython.org/github/oplatek/pykaldi-eval/blob/master/Pykaldi-evaluation.ipynb.

An example posterior word lattice output for one Czech utterance can be seen at http://oplatek.blogspot.it/2014/02/ipython-demo-pykaldi-decoders-on-short.html

Install
-------

..  image:: https://travis-ci.org/UFAL-DSG/pykaldi.png
    :target: https://travis-ci.org/UFAL-DSG/pykaldi


* Read `INSTALL.rst <./INSTALL.rst>`_ and `INSTALL <./INSTALL>`_ first!
* `INSTALL.rst <./INSTALL.rst>`_ contains instructions specific for this fork. 
  `INSTALL <./INSTALL>`_ stores general instructions for Kaldi.


LICENSE
--------
* This Kaldi fork is released under the `Apache license, Version 2.0 <http://www.apache.org/licenses/LICENSE-2.0>`_, which is also used by Kaldi itself. 
* We also publicly released Czech and English data for ``kaldi/egs/vystadial`` recipe under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) license:

  * Czech data https://lindat.mff.cuni.cz/repository/xmlui/handle/11858/00-097C-0000-0023-4670-6
  * English data https://lindat.mff.cuni.cz/repository/xmlui/handle/11858/00-097C-0000-0023-4671-4

  Note that the data are automatically downloaded in the recipe scripts.

Other info
----------
* This Kaldi fork is developed under `Vystadial project <https://sites.google.com/site/filipjurcicek/projects/vystadial>`_.
* Based on the `Svn trunk of Kaldi project <svn://svn.code.sf.net/p/kaldi/code/trunk>`_ which is mirrored to branch ``svn-mirror``.
* The svn trunk is mirrored via ``git svn``. 
  Checkout tutorials: `Git svn <http://viget.com/extend/effectively-using-git-with-subversion>`_, 
  `Svn branch in git <http://ivanz.com/2009/01/15/selective-import-of-svn-branches-into-a-gitgit-svn-repository>`_
