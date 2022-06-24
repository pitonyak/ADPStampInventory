#! /usr/bin/env python3

#
# This imports the file / module "encrypted.py"
# If I place this after the other imports, then the import fails. 
#
import encrypted as gh

from argparse import ArgumentParser
#from collections import deque   # A fast queue not meant for use with threading
from OuiLookup import OuiLookup
from re import L
from scapy.all import *
from scapy.contrib import ospf
import copy
import ipaddress
import math
import networkx as nx
import numpy
import os
import scipy.fftpack as sff
import scipy.special as spc
import scipy.stats as sst
import time
import unittest

# See https://docs.python.org/3/library/unittest.html

class TestStringMethods(unittest.TestCase):

    def test_upper(self):
        self.assertEqual('foo'.upper(), 'FOO')

    def test_isupper(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

class TestBinaryMethods(unittest.TestCase):

    def test_count_ones(self):
    	# 0 .. 255
    	for i in range(256):
    	    ones = len(bin(i)[2:].replace("0", ""))
    	    self.assertEqual(ones, gh.ones_in_a_byte(i))

    def test_count_zeros(self):
    	# 0 .. 255
    	for i in range(256):
    	    zeros = len('{0:08b}'.format(i).replace("1", ""))
    	    self.assertEqual(zeros, gh.zeroes_in_a_byte(i))

    def test_bit_string(self):
    	# 0 .. 255
    	for i in range(256):
    	    bits = '{0:08b}'.format(i)
    	    self.assertEqual(bits, gh.byte_to_bits(i))


if __name__ == '__main__':
    unittest.main()



