#!/bin/bash
#Copyright 2013 Ufal MFF UK; Ondrej Platek
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#http://www.apache.org/licenses/LICENSE-2.0
#
#THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
#WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
#MERCHANTABLITY OR NON-INFRINGEMENT.
#See the Apache 2 License for the specific language governing permissions and
#limitations under the License.
#
# This script installs PyOpenFST 
# https://code.google.com/p/pyopenfst/

echo "**** Installing PyOpenFST and dependencies"

echo "Checking for Python-Dev"
# copied from http://stackoverflow.com/questions/4848566/check-for-existence-of-python-dev-files-from-bash-script
if [ ! -e $(python -c 'from distutils.sysconfig import get_makefile_filename as m; print m()') ]; then 
    echo "On Debian/Ubuntu like system install by 'sudo apt-get python-dev' package."
    echo "On Fedora by 'yum install python-devel'"
    echo "On Mac OS X by 'brew install python'"
    echo "Or obtain Python headers other way!"
    exit 1
fi

# install openfst first
make openfst || exit 1;

prefix="${PWD}/python"
dir=pyopenfst
fstdir=${PWD}/openfst
[ -d "$fstdir" ] || exit 1;

hg clone https://code.google.com/p/pyopenfst/ "$dir"
pushd $dir
python setup.py build_ext --include-dirs "${fstdir}/include" --library-dirs "${fstdir}/lib"
python setup.py install --prefix=${prefix}
popd


