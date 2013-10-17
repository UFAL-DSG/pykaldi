#!/usr/bin/env python
# encoding: utf-8


from setuptools import setup, find_packages
from sys import version_info as python_version
from os import path
from pykaldi import ffidec


install_requires = ['cffi >=0.7']  # cffi transitively requires pycparser


if python_version < (2, 7):
    new_27 = ['ordereddict', 'argparse']
    install_requires.extend(new_27)

long_description = open(path.join(path.dirname(__file__), 'README.md')).read()

setup(
    name='pykaldi',
    version='0.0',
    install_requires=install_requires,
    packages=find_packages(exclude=["*.tests", "*.tests.*", "tests.*", "tests"]),
    # based on cffi docs: http://cffi.readthedocs.org/en/release-0.7/
    zip_safe=False,
    # get extension from cffi if using verify
    ext_package='pykaldi',
    ext_modules=[ffidec.verifier.get_extension()],
    test_suite="nose.collector",
    tests_require=['nose>=1.0', 'pykaldi'],
    entry_points={
        'console_scripts': [
            'live_demo=pykaldi.binutils.main',
            'online_decode=pykaldi.binutils.main',
        ],
    },
    author='Ondrej Platek',
    author_email='ondrej.platek@seznam.cz',
    url='https://github.com/oplatek/pykaldi',
    license='Apache, Version 2.0',
    keywords='Kaldi speech recognition Python bindings',
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
