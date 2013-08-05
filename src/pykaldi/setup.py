#!/usr/bin/env python
# encoding: utf-8


from setuptools import setup, find_packages
from sys import version_info
from os import path


install_requires = [
    'cffi >=0.6',
    'pycparser >= 2.9.1']


if version_info < (2, 7):
    new_in27 = ['ordereddict', 'argparse']
    install_requires.extend(new_in27)

long_description = open(path.join(path.dirname(__file__), 'README.md')).read()

setup(
    name='pykaldi',
    version='0.0',
    install_requires=install_requires,
    packages=find_packages(),
    zip_safe=False,  # based on cffi docs
    author='Ondrej Platek',
    author_email='ondrej.platek@seznam.cz',
    url='https://github.com/oplatek/pykaldi',
    license='Apache, Version 2.0',
    keywords='kaldi speech recognition python bindings',
    description='C and Python wrapper for Kaldi decoders',
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
