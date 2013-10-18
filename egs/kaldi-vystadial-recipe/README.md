Summary
-------
 * KALDI recipe based on voxforge KALDI recipe http://vpanayotov.blogspot.cz/2012/07/voxforge-scripts-for-kaldi.html .
 * Requires KALDI installation and Linux environment. (Tested on Ubuntu 10.04, 12.04 and 12.10.)
 * Written in Bash an Python 2.7.3.



Details
-----------
 * Our scripts prepare the data to expected format in s5/data. 
 * Stores experiments in s5/exp
 * steps/ contains common scripts from wsj/s5/utils
 * utils/ cotains common scritps from wsj/s5/utils
 * local/ contains scripts for data preparation to prepare s5/data structure
 * path.sh, cmd.sh and  conf/ contain configurations for the recipe
 * Language Model (LM) is either built from the training data using [IRSTLM](http://sourceforge.net/projects/irstlm/)  or we supply one in ARPA format


Runnning experiments
--------------------
Before running the experiments check the following files:
 * `conf` directory contains different configuration related for the training
 * `path.sh` just set up path for running Kaldi binaries and path to data.
    You should also setup `njobs` according your computer capabalities.
 * `cmd.sh` set training commands e.g. for SGE grid.
 * If you set up everything right, just launch `run.sh` It will create `mfcc`, `data` and `exp` directories.
   If any of them exists, it will ask you if you want them to be overwritten.
 ```bash
 ./run.sh | tee mylog.log # I always store the output to the log
 ```
