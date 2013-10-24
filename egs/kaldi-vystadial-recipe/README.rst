Summary
-------
 * This KALDI recipe is based on Voxforge KALDI recipe http://vpanayotov.blogspot.cz/2012/07/voxforge-scripts-for-kaldi.html .
 * Requires KALDI installation and Linux environment. (Tested on Ubuntu 10.04, 12.04 and 12.10.)
 * Recipes Kaldi toolkit are located `KALDI_ROOT/egs/name_of_recipe/s5/`. 
   This recipe also expects that you copy the directory which contains this `README.rst` under `KALDI_ROOT/egs`,
   or that you have Kaldi binaries and in your path.



Details
-----------
 * Our scripts prepare the data to expected format in s5/data
 * Stores experiments in s5/exp
 * steps is a symlink to `KALDI_ROOT/wsj/s5/utils`
 * utils is a symlink to `KALDI_ROOT/wsj/s5/utils`
 * Directory `local` contains scripts for data preparation to prepare `s5/data` structure.
 * `path.sh`, `cmd.sh` and  `conf/*` contain configurations for the recipe
 * Language Model (LM) is either built from the training data using [IRSTLM](http://sourceforge.net/projects/irstlm/)  or we supply one in ARPA format


Running experiments
--------------------
Before running the experiments check following points:
 * You have compiled Kaldi toolkit http://sourceforge.net/projects/kaldi/
 * You have compiled IRSTLM. (For building a language model (LM) if you don't supply your own LM in arpa format) http://sourceforge.net/projects/irstlm/
 * The run.sh script will see the Kaldi binaries. There are two options:
    - Copy this directory into `KALDI_ROOT/egs`. (Recommended)
    - The Kaldi binaries are in your `PATH` variable and fix the `utils` and `steps` symlinks.
 * Check path links in `conf` directory to data and that the set up fits your needs. 
 * Check `path.sh` for path to Kaldi binaries and number of jobs parameter, `njobs`. 
 * In `cmd.sh` you can switch to run the training on SGE grid (disabled by default).

Start the recipe from `s5` directory by running `bash run.sh`.
It will create `mfcc`, `data` and `exp` directories.
If any of them exists, it will ask you if you want them to be overwritten.
After running the experiments the `exp` directory will be backup to `Results` directory.

Extracting the results and trained models
-------------------------------------------
The main `s5/run.sh` script performs not only training the acoustic models,
but also decoding.
The acoustic models are evaluated during running the scripts and are printed to the standard output.

The `local/results.py exp` command extracts the results from the `exp` directory.
It is invoked at the end of the `s5/run.sh` script and the results are stored to `exp/results.log`.

If you want to use the trained acoustic model outside the prepared script,
you need to build `HCLG` decoding graph yourself. 
(See http://kaldi.sourceforge.net/graph.html for general introduction to FST framework in Kaldi.)

The simplest way how to start with decoding is to use the same LM which
was used by the `s5/run.sh` script.  
Let say you want to decode with acoustic model stored in `exp/tri1`,
then you need just 3 files:
    - `exp/tri1/graph/HCLG.fst`   # decoding graph
    - `exp/tri1/graph/words.txt`  # Word symbol table, a mapping between words and integers which are decoded.
    - `exp/tri1/final.mdl`        # trained acoustic model 

For details see the `s5/run.sh` script and especially the parts where the `steps/decode.sh` is used. 
The `steps/decode.sh` wraps the decoding with Kaldi binaries.
