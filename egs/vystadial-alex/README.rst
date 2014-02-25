Summary
-------
* Requires Kaldi installation and Linux environment. (Tested on Ubuntu 10.04, 12.04 and 12.10.)
* Trained models described in the paper are available in the 
  `model_voip_cs` and `model_voip_en` directories for default configuration.


Details
-------
* The config files `s5/env_voip_{cs,en}.sh` sets the data directory,
  mfcc directory and experiments directory.
  The default configuration `$WORK/model_voip_{cs,en}, 
  $WORK/model_voip_{cs,en}/exp, $WORK/mfcc`.
* Our scripts prepare the data to the expected format in `s5/$WORK/data`.
* Experiment files are stored to `$exp` directory e.g. `data_voip_cs/exp`.
* The `local` directory contains scripts for data preparation to prepare 
  lang directory.
* `path.sh`, `cmd.sh` and  `common/*` contain configurations for the 
  recipe.
* Language model (LM) is either built from the training data using 
  [SRILM](http://www.speech.sri.com/projects/srilm/)  or we supply one in 
  the ARPA format.


Running experiments
-------------------
Before running the experiments, check that:

* you have the Kaldi toolkit compiled: 
  http://sourceforge.net/projects/kaldi/.
* you have SRILM compiled. (This is needed for building a language model 
  unless you supply your own LM in the ARPA format.) 
  See http://www.speech.sri.com/projects/srilm/.
* The number of jobs `njobs` are set correctly in `path.sh`.
* In `cmd.sh`, you switched to run the training on a SGE[*] grid if 
  required (disabled by default).

Start the recipe from the `s5` directory by running 
`bash train_voip_cs.sh` or `bash train_voip_en.sh`.
It will create `$WORK/mfcc`, `$WORK/data` and `$WORK/exp` directories.
If any of them exists, it will ask you if you want them to be overwritten.

.. [*] Sun Grid Engine

Extracting the results and trained models
-----------------------------------------
The main scripts, `s5/train_voip_{cs,en}.sh`, 
perform not only training of the acoustic 
models, but also decoding.
The acoustic models are evaluated after running the training and  
reports are printed to the standard output.

The `s5/local/results.py exp` command extracts the results from the `$exp` directory.
It is invoked at the end of the `s5/train_voip_{cs,en}.sh` script and 
the results are thereby stored to `$WORK/exp/results.log`.

If you want to use the trained acoustic model with your language model
outside the prepared script, you need to build the `HCLG` decoding graph yourself.  
See http://kaldi.sourceforge.net/graph.html for general introduction to the FST 
framework in Kaldi.

The simplest way to start decoding is to use the same LM which
was used by the `s5/train_voip_{cs,en}.sh` script.
Let's say you want to decode with 
the acoustic model stored in `exp/tri2b_bmmi`,
then you need files listed below:

----

====================================  ====================================================================================
`mfcc.conf`                         # Speech parametrisation (MFCC) settings. Training and decoding setup must match.
`exp/tri2b_bmmi/graph/HCLG.fst`     # Decoding Graph. Graph part of AM plus lexicon, phone->3phone & LM representation.
`exp/tri2b_bmmi/graph/words.txt`    # Word symbol table, a mapping between words and integers which are decoded.
`exp/tri2b_bmmi/graph/silence.csl`  # List of phone integer ids, which represent silent phones. 
`exp/tri2b_bmmi/final.mdl`          # Trained acoustic model (AM).
`exp/tri2b_bmmi/final.mat`          # Trained matrix of feature/space transformations (E.g. LDA and bMMI).
====================================  ====================================================================================


----

We recommend to study `steps/decode.sh` Kaldi standard script
for standalone decoding with `gmm-latgen-faster` Kaldi decoder.

In order to build your own decoding graph `HCLG` 
you need LM in ARPA format and files in table below. 

* Note 1: Building `HCLG` decoding graph is out of scope this README.
* Note 2: Each acoustic model needs corresponding `HCLG` graph.
* Note 3: The phonetic dictionary applied on the vocabulary 
  should always generate only a subset of phones seen in training data!

====================================  ====================================================================
`LM.arpa`                           # Language model in ARPA format [You should supply it]
`vocabulary.txt`                    # List of words you want to decode [You should supply it]
`OOV_SYMBOL`                        # String representing out of vocabulary word. [You should supply it]
`dictionary.txt`                    # Phonetic dictionary. [You should supply it]
`exp/tri2b_bmmi/final.mdl`          # Trained acoustic model (AM).
`exp/tri2b_bmmi/final.tree`         # Phonetic decision tree.
====================================  ====================================================================

Note: We supply the LDA+bMMI acoustic model, 
its decoding graph and support files
with the Kaldi scripts.
Using the two tables above their purpose should be obvious.

Expected results
----------------

.. code-block:: bash

    build2 - bigram LM from train data, estimated by the scripts using SRILM
    build0 - zerogram LM from test data, estimated by scripts using Python code
    LMW - Language model weight, we picked the best from (min_lmw, max_lmw)
          based on decoding results on DEV set

    Full Czech data: 
    exp             set     LM      LMW     WER     SER  
    mono            test    build0  6       86.1    89.66
    tri1            test    build0  8       70.84   82.9 
    tri2a           test    build0  8       70.86   83.01
    tri2b           test    build0  9       68.13   80.89
    tri2b_mmi       test    build0  9       67.61   79.53
    tri2b_mmi_b0.05 test    build0  8       66.18   78.72
    tri2b_mpe       test    build0  9       64.93   77.66
    mono            test    build2  8       72.3    79.02
    tri1            test    build2  11      55.57   72.11
    tri2a           test    build2  11      55.12   70.9 
    tri2b           test    build2  12      52.95   70.7 
    tri2b_mmi       test    build2  10      50.42   68.38
    tri2b_mmi_b0.05 test    build2  10      49.96   68.58
    tri2b_mpe       test    build2  12      49.87   66.97

    Full English data:
    exp             set     LM      LMW     WER     SER
    mono            test    build0  9       67.52   91.6
    tri1            test    build0  10      36.75   74.7
    tri2a           test    build0  10      35.8    71.65
    tri2b           test    build0  10      32.24   68.35
    tri2b_mmi       test    build0  9       24.36   54.5
    tri2b_mmi_b0.05 test    build0  9       23.72   53.1
    tri2b_mpe       test    build0  10      25.81   59.45
    mono            test    build2  14      31.51   64.2
    tri1            test    build2  20      15.2    43.55
    tri2a           test    build2  16      15.61   43.4
    tri2b           test    build2  19      15.27   42.8
    tri2b_mmi       test    build2  14      10.2    30.45
    tri2b_mmi_b0.05 test    build2  17      10.09   29.85
    tri2b_mpe       test    build2  20      15.54   42.2

    Note that the zero-gram LMs for discriminative training
    give significant advantage, because they are estimated on test set!


Credits and license
------------------------
The scripts are partialy based on Voxforge KALDI recipe.
The original scripts as well as theses scripts are licensed under APACHE 2.0 license.
The data are distributed under Attribution-{ShareAlike} 3.0 Unported ({CC} {BY}-{SA} 3.0) license.

The data collecting process was funded by project Vystadial.
For citing, please use following ``BibTex`` citation:


.. code-block:: tex

    @todo{todo_lrec_2014,
      author = {Korvas, Matěj; Pl\'{a}tek, Ondřej; Du\v{s}ek, Ondřej; \v{Z}ilka, Luk\'{a}\v{s}; Jur\v{c}\'{i}\v{c}ek, Filip},
      title = {Free English and Czech telephone speech corpus shared under the CC-BY-SA 3.0 license},
      year = {2014},
      language = {eng},
      institution = {Faculty of Mathematics and Physics, Charles University in Prague, {UFAL}}
    }
