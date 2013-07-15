Known bugs
==========
 * The demo was tested with `Python 2.7.*` and should run for `Python 2.6.6` as well
 * Currently the demo *NEEDS Numpy INSTALLED* ! That dependency will be removed very shortly however!
    [Building and installing Numpy](http://docs.scipy.org/doc/numpy/user/install.html)


Report the bugs at [https://github.com/oplatek/pykaldi/issues/](https://github.com/oplatek/pykaldi/issues/)
or send the reports to me via email.

Thanks!


How to run the demo
===================
```bash
# 1. obtain source code:
svn checkout svn+ssh://oplatek@svn.code.sf.net/p/kaldi/code/sandbox/oplatek2  # Change your username
# or probably more recent
git clone git@github.com:oplatek/pykaldi.git oplatek2

# 2. INSTALL portaudio and cffi. 
# For portaudio:
cd oplatek2/tools; ./install_portaudio.sh
# For cffi you have options a) or b)
# a) Go to http://cffi.readthedocs.org/en/latest/ and following the instructions install the cffi system wide! (Recommended)
#    Read the Requirements section!
# b) Go to oplatek2/tools and install cffi locally by using install_cffi.sh. 
# After a successful installation the script prompts you to add the installation directory to PYTHONPATH. 
# Do it, it will be needed in step 7.

# 3. Update PYTHONPATH
# if using 2b) The installation script will give you statement like the one below! So use it! 
export PYTHONPATH="$PYTHONPATH:/ha/work/people/oplatek/kaldi/tools/python/lib/python2.6/site-packages"
# add src/pykaldi directory itself to PYTHONPATH. Use absolute paths like this!
export PYTHONPATH="$PYTHONPATH":/home/oplatek/kaldi-svn/oplatek2/src

# 4. 
cd oplatek2/src

# 5. Configure it with --shared flag
 ./configure --fst-root=`pwd`/../tools/openfst --shared

# 6. Build Kaldi. Clean it and tested to be sure that, it is not corrupted.
make clean; make depend && make ext_depend && make && make ext && make test && make ext_test

# 7. Change to the directory with the example.
cd pykaldi/binutils/

# 8. Run make test. It should compile and download everything needed
make test

# 9. Check the results! My results for python-online-wav-gmm-decode-faster are:

python-compute-wer --config=configs/wer.config ark:work/reference.txt ark:work/online.trans.compact 
%WER 15.57 [ 57 / 366, 8 ins, 15 del, 34 sub ]
%SER 100.00 [ 3 / 3 ]
Scored 3 sentences, 0 not present in hyp.
```
