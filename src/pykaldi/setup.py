#!/usr/bin/env python
# encoding: utf-8
# On Windows, you need to execute:
# set VS90COMNTOOLS=%VS100COMNTOOLS%
# python setup.py build_ext --compiler=msvc
from setuptools import setup
from sys import version_info as python_version
from os import path
from Cython.Distutils import build_ext
from distutils.extension import Extension

include_dirs = []
library_dirs = [path.abspath('../lib')]

install_requires = []
if python_version < (2, 7):
    new_27 = ['ordereddict', 'argparse']
    install_requires.extend(new_27)

long_description = open(path.join(path.dirname(__file__), 'README.md')).read()

ext_modules = [Extension('decoders',
                         language='c++',
                         include_dirs=['..', ],
                         library_dirs=['../dec-wrap'],
                         libraries=['pykaldi'],
                         # extra_objects=['../dec-wrap/pykaldi.a'],
                         sources=['pykaldi/decoders.pyx'],
                         ), ]


setup(
    name='pykaldi',
    version='0.0',
    cmdclass={'build_ext': build_ext},
    install_requires=install_requires,
    setup_requires=['cython>=0.19.1'],
    ext_package='pykaldi',
    ext_modules=ext_modules,
    test_suite="nose.collector",
    tests_require=['nose>=1.0', 'pykaldi'],
    # entry_points={
    #     'console_scripts': [
    #         'live_demo=pykaldi.binutils.main',
    #     ],
    # },
    author='Ondrej Platek',
    author_email='ondrej.platek@seznam.cz',
    url='https://github.com/oplatek/pykaldi',
    license='Apache, Version 2.0',
    keywords='Kaldi speech recognition Python bindings',
    description='C++/Python wrapper for Kaldi decoders',
    long_description=long_description,
    classifiers='''
        Programming Language :: Python :: 2
        License :: OSI Approved :: Apache License, Version 2
        Operating System :: POSIX :: Linux
        Intended Audiance :: Speech Recognition scientist
        Intended Audiance :: Students
        Environment :: Console
        '''.strip().splitlines(),
)
