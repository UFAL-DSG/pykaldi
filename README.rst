Fork of Kaldi for developing custom recognisers for Alex spoken dialogue system framework
=========================================================================================

News & info
-----------

* We use `Docker <https://registry.hub.docker.com/search?q=ufaldsg/pykaldi>`_ so you can try easily our decoding demo

    * Run the demo using the two commands:

        1. download image ``docker pull ufaldsg/pykaldi``
        2. run the demo ``docker run ufaldsg/pykaldi /bin/bash -c "cd online_demo; make gmm-latgen-faster; make online-recogniser; make pyonline-recogniser"``

            - Note the demo downloads the pretrained models and test data which you may safe using ``docker commit`` functionality 

    * Start exploring the demo source codes `online_demo/pykaldi-online-latgen-recogniser.py <https://github.com/UFAL-DSG/pykaldi/blob/master/online_demo/pykaldi-online-latgen-recogniser.py>`_ and `onl-rec/onl-rec-latgen-recogniser-demo.cc <https://github.com/UFAL-DSG/pykaldi/blob/master/onl-rec/onl-rec-latgen-recogniser-demo.cc>`_
    * Please note, that you need to change the source code of Pykaldi in the docker image to effect the demo behaviour when using docker.

* The Python wrapper of C++ ``OnlineLatticeRecogniser`` implements  MFCC, LDA+MLLT, bMMI acoustic models since it was the best speaker independent setup.
* UPDATE: Since 11/18/2014 the Pykaldi fork uses the Kaldi official code (``src/online2``) which has very similar as our previous implementation (and was finished roughly 8 month after our implementations).


Install
-------

.. image:: https://travis-ci.org/UFAL-DSG/pykaldi.svg?branch=master
    :target: https://travis-ci.org/UFAL-DSG/pykaldi

* Our priority is to deploy it on Ubuntu 14.04 and also keep Travis running on Ubuntu 12.04
* Read `INSTALL.rst <./INSTALL.rst>`_ and `INSTALL <./INSTALL>`_ first!
* `INSTALL.rst <./INSTALL.rst>`_ contains instructions specific for this fork. 
  `INSTALL <./INSTALL>`_ stores general instructions for Kaldi.


LICENSE
--------
* This Kaldi fork is released under the `Apache license, Version 2.0 <http://www.apache.org/licenses/LICENSE-2.0>`_, which is also used by Kaldi itself. 
* We also publicly released Czech and English data for ``kaldi/egs/vystadial_{cz,en}`` recipe under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) license:

  * Czech data https://lindat.mff.cuni.cz/repository/xmlui/handle/11858/00-097C-0000-0023-4670-6
  * English data https://lindat.mff.cuni.cz/repository/xmlui/handle/11858/00-097C-0000-0023-4671-4

  Note that the data are automatically downloaded in the recipe scripts.

History
-------
The fork presented three new Kaldi features in thesis of Ondrej Platek (see commit 8e534b16bb8a350):
* Training scripts which can be used with standard Kaldi tools or with the new ``OnlineLatticeRecogniser``.
  The scripts for Czech and English support acoustic models obtained using MFCC, LDA+MLLT/delta+delta-delta feature transformations and acoustic models trained generatively or by MPE or bMMI training.

The new functionality was separated to different directories:
 * pykaldi/src/onl-rec stores C++ code for ``OnlineLatticeRecogniser``.
 * pykaldi/pykaldi stores Python wrapper ``PyOnlineLatticeRecogniser``.
 * kaldi/egs/vystadial_{cz,en}/s5 stores training scripts. [merged to oficial Kaldi repo]
 * kaldi/online_demo shows Kaldi standard decoder, ``OnlineLatticeRecogniser`` and ``PyOnlineLatticeRecogniser``, which produce the exact same lattices using the same setup.

The ``OnlineLatticeRecogniser`` is used in Alex dialogue system (https://github.com/UFAL-DSG/alex).

In March 2014, the ``PyOnlineLatticeRecogniser`` recogniser was evaluated on Alex domain.
See graphs evaluating ``OnlineLatticeRecogniser`` performance at http://nbviewer.ipython.org/github/oplatek/pykaldi-eval/blob/master/Pykaldi-evaluation.ipynb.

An example posterior word lattice output for one Czech utterance can be seen at http://oplatek.blogspot.it/2014/02/ipython-demo-pykaldi-decoders-on-short.html


Other info
----------
* This Kaldi fork is developed under `Vystadial project <https://sites.google.com/site/filipjurcicek/projects/vystadial>`_.
* Based on the `Svn trunk of Kaldi project <svn://svn.code.sf.net/p/kaldi/code/trunk>`_ which is mirrored to branch ``svn-mirror``.
* The svn trunk is mirrored via ``git svn``. 
  Checkout tutorials: `Git svn <http://viget.com/extend/effectively-using-git-with-subversion>`_, 
  `Svn branch in git <http://ivanz.com/2009/01/15/selective-import-of-svn-branches-into-a-gitgit-svn-repository>`_
