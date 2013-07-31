#!/usr/bin/env python
# Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
# WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
# MERCHANTABLITY OR NON-INFRINGEMENT.
# See the Apache 2 License for the specific language governing permissions and
# limitations under the License. #

import unittest
import copy
from utils import expand_prefix


class TestExpandPref(unittest.TestCase):
    def setUp(self):
        self.test = {'x': '1',
                     'mypath': {'prefix': 'myownpath/', 'value': 'origin'},
                     'y': {'prefix': 'mypath', 'value': 'file'},
                     'innerdic': {'a': {'prefix': 'mypath', 'value': ''}, 'b': ['arg1', 'arg2']},
                     'innerlist': [{'prefix': 'mypath', 'value': 'file2'}, 'arg3']
                     }
        self.gold = {'x': '1',
                     'mypath': 'myownpath/origin',
                     'y': 'myownpath/file',
                     'innerdic': {'a': 'myownpath/', 'b': ['arg1', 'arg2']},
                     'innerlist': ['myownpath/file2', 'arg3']
                     }

    def test_expand_prefix(self):
        test, gold = copy.deepcopy(self.test), copy.deepcopy(self.gold)
        expand_prefix(test, gold)

        self.assertTrue(self.gold == gold, 'We modified gold!')

        self.assertFalse(set(test) ^ set(gold), 'symetric difference of keys should be empty')
        self.assertTrue(gold['y'] == test['y'], 'prefix expansion  fails')
        t, g = gold['innerdic']['a'], test['innerdic']['a']
        self.assertTrue(t == g, 'empty value for expansion fails: %r vs %r' % (t, g))
        self.assertTrue(gold['innerlist'][0] == test['innerlist'][0], 'list expansion fails')

        print '%r\nvs\n%r\n' % (test, gold)


if __name__ == '__main__':
    unittest.main()
