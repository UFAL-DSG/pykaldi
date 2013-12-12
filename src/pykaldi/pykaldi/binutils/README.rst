* For drawing you need `dot` binary from graph [http://www.graphviz.org/Download..php](graphviz) library.
DEPRECATED - TODO REWRITE
=========================


Known bugs
==========
 * Currently not working and this walkthrough is DEPRECATED

Report the bugs at [https://github.com/oplatek/pykaldi/issues/](https://github.com/oplatek/pykaldi/issues/)
or send the reports to me via email.

Thanks!


How to run the demo
===================
```bash
# 1. obtain source code:
git clone git@github.com:oplatek/pykaldi.git oplatek2

2.
#    Read the Requirements section!
# b) Go to oplatek2/tools and install cffi locally by using install_cffi.sh. 
# After a successful installation the script prompts you to add the installation directory to PYTHONPATH. 
# Do it, it will be needed in step 7.

# 3. Update PYTHONPATH
# if using 2b) The installation script will give you statement like the one below! So use it! 
export PYTHONPATH="$PYTHONPATH:/ha/work/people/oplatek/kaldi/tools/python/lib/python2.7/site-packages"
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

# 8 a) Run a demo which downloads sample acoustic and language models
with testing audio data and run the decoding.
make pykaldi_latgen

# 8. b) If you have a microphone you can try to talk to live_demo.
It should compile and download everything needed
make run_live 
