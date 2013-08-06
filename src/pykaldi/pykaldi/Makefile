# FIXME use setup.py utils  for testing
# see http://parijatmishra.wordpress.com/2008/10/08/python-packaging-setuptools-and-eggs/
# and http://pythonhosted.org/distribute/setuptools.html#tagging-and-daily-build-or-snapshot-releases

decoders.py: __init__.py

decoders_test.py: __init__.py

# extracts testing data
DECODE_DIR = work
include ../online-data.mk

test: decoders_test.py $(DATA_TAR)
	nosetests -s $<  # -s let the output be printed

clean:
	rm -rf $(DATA_TAR) $(DATA_FILE) $(DECODE_DIR) *.pyc  __pycache__

.PHONY: test clean
