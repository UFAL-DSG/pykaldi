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
import yaml
import pystache


install_requires = []
if python_version < (2, 7):
    new_27 = ['ordereddict', 'argparse']
    install_requires.extend(new_27)


ext_modules = []
ext_modules.append(Extension('decoders',
                             language='c++',
                             include_dirs=['..'],
                             library_dirs=['../dec-wrap'],
                             libraries=['pykaldi'],
                             # extra_objects=['../dec-wrap/pykaldi.a'],
                             sources=['pykaldi/decoders.pyx'],
                             ))


templates = [
    ('fst/_fst.pyx.tpl', 'fst/types.yml', 'fst/_fst.pyx'),
    ('fst/libfst.pxd.tpl', 'fst/types.yml', 'fst/libfst.pxd'),
]


class pre_build_ext(build_ext):
    def run(self):
        '''Before building the C++ extension apply the
        templates substitution'''
        print 'running pre_build_ext'
        try:
            for templ_name, dic_name, result in templates:
                with open(dic_name, 'r') as d:
                    with open(templ_name, 'r') as t:
                        with open(result, 'w') as r:
                            dic = yaml.load(d)
                            tmpl = t.read()
                            r.write(pystache.render(tmpl, dic))
                            print 'Created template %s' % result
            build_ext.run(self)
        except Exception as e:
            # how to handle bad cases!
            print e
            raise e

ext_modules.append(Extension(name='fst._fst',
                             sources=['fst/_fst.pyx'],
                             language='c++',
                             include_dirs=[],
                             libraries=['fst'],
                             library_dirs=[],
                             ))

long_description = open(path.join(path.dirname(__file__), 'README.rst')).read()

setup(
    name='pykaldi',
    version='0.0',
    cmdclass={'build_ext': pre_build_ext},
    install_requires=install_requires,
    setup_requires=['cython>=0.19.1', 'pyyaml', 'pystache'],
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
