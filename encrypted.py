#! /usr/bin/env python3
from argparse import ArgumentParser
#from collections import deque   # A fast queue not meant for use with threading
#from re import L
import copy
#import ipaddress
import math
#import networkx as nx
import os
from os.path import exists
import time
import csv
import io
import scipy.fftpack as sff
import scipy.special as spc
import scipy.stats as sst
import numpy
from scapy.all import *
#from scapy.contrib import ospf

#
# What do we want in our output CSV file?
#
# Each test has a numeric value assocaited with it.
# The confidence level (probably 0.005) determines if a test passes or failes.
# A value less than (<) the confidence level fails.
# A value of -1 means that the test was skipped.
#

"""
Column  Header  Description
0   Index   zero based index to the packet in the pcap file.
1   Percent Percent Passed
2   Num Passed  Number of tests that passed
3   Num Skipped Number of tests skipped because they were considered not appropriate for the test
4   Num Failed  Number of tests that failed the randomness test
5   Confidence Level    Confidence level used for the tests, probably 0.005
6   Monobit Test
7   Block Frequency Test
8   Independent Runs Test
9   Longest Runs Test
10  Matrix Rank Test
11  Spectral Test
12  Non Overlapping Patterns Test
13  Overlapping Patterns Test
14  Universal Test
15  Linear Complexity Test
16  Serial Test (p01)
17  Serial Test (p02)
18  Approximate Entropy Test
19  Cumulative Sums Test (Forward)
20  Cumulative Sums Test (Backward)
21  Random Excursions Test (p01)
22  Random Excursions Test (p02)
23  Random Excursions Test (p03)
24  Random Excursions Test (p04)
25  Random Excursions Test (p05)
26  Random Excursions Test (906)
27  Random Excursions Test (p07)
28  Random Excursions Test (p08)
29  Random Excursions Variant Test (p01)
30  Random Excursions Variant Test (p02)
31  Random Excursions Variant Test (p03)
32  Random Excursions Variant Test (p04)
33  Random Excursions Variant Test (p05)
34  Random Excursions Variant Test (p06)
35  Random Excursions Variant Test (p07)
36  Random Excursions Variant Test (p08)
37  Random Excursions Variant Test (p09)
38  Random Excursions Variant Test (p10)
39  Random Excursions Variant Test (p11)
40  Random Excursions Variant Test (p12)
41  Random Excursions Variant Test (p13)
42  Random Excursions Variant Test (p14)
43  Random Excursions Variant Test (p15)
44  Random Excursions Variant Test (p16)
45  Random Excursions Variant Test (p17)
46  File    PCAP file name
47  Source IP
48  Dest IP
49  Data Len
50  Data        - First few bytes of the data.
"""

#
# Packet types: https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
#
# Encap Security Payload (ESP) Packet is type 50
# see https://www.rfc-editor.org/rfc/rfc4303.html for ESP information
# See https://en.wikipedia.org/wiki/IPsec#Encapsulating_Security_Payload for ESP information
#
# I anticipate the following fields:
#   nh         - IP_PROTOS
#   payloadlen - length of the ICV in 32-bit (4 byte) words.
#   icv        - ?? the data maybe
#   padding    - The padding maybe, get the layer perhaps.
#   payload_guess ??
#   looks to be a "data" field, which might be the entire ESP packet.
#
# To prepare to run, I install the following items:
#
# Note: PyX allows for a PDF dump, and it assumes that certain things are installed:
# apt install texlive-latex-base
# dnf install texlive-latex-base-dev.noarch
#
# python3 -m venv /andrew0/home/andy/Documents/Battelle/GreenHornet/Random_src/random
# source /andrew0/home/andy/Devsrc/Battelle/GreenHornet/Random_src/random/bin/activate
# pip install numpy scipy pandas quandl bitstring pycrypto
# pip install seaborn scapy PyX networkx
# cd /andrew0/home/andy/Devsrc/Battelle/GreenHornet/pcap
#
# TODO: Remove this comment after testing and posting.
# Test files containing encrypted packets:
# 1     wireshark/fuzz-2006-07-05-11195.pcap
# 2     wireshark/fuzz-2006-07-05-1209.pcap (that data does NOT look random!)
# 4     wireshark/fuzz-2006-07-06-20811.pcap
# 3     wireshark/fuzz-2006-07-06-25704.pcap
# 4     wireshark/fuzz-2006-07-06-5536.pcap
#
#
#
# Use this to count the number of bits (ones) in a byte.
#
ONE_COUNTS = (b'\x00\x01\x01\x02\x01\x02\x02\x03\x01\x02\x02\x03\x02\x03\x03\x04'
          b'\x01\x02\x02\x03\x02\x03\x03\x04\x02\x03\x03\x04\x03\x04\x04\x05'
          b'\x01\x02\x02\x03\x02\x03\x03\x04\x02\x03\x03\x04\x03\x04\x04\x05'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x01\x02\x02\x03\x02\x03\x03\x04\x02\x03\x03\x04\x03\x04\x04\x05'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x03\x04\x04\x05\x04\x05\x05\x06\x04\x05\x05\x06\x05\x06\x06\x07'
          b'\x01\x02\x02\x03\x02\x03\x03\x04\x02\x03\x03\x04\x03\x04\x04\x05'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x03\x04\x04\x05\x04\x05\x05\x06\x04\x05\x05\x06\x05\x06\x06\x07'
          b'\x02\x03\x03\x04\x03\x04\x04\x05\x03\x04\x04\x05\x04\x05\x05\x06'
          b'\x03\x04\x04\x05\x04\x05\x05\x06\x04\x05\x05\x06\x05\x06\x06\x07'
          b'\x03\x04\x04\x05\x04\x05\x05\x06\x04\x05\x05\x06\x05\x06\x06\x07'
          b'\x04\x05\x05\x06\x05\x06\x06\x07\x05\x06\x06\x07\x06\x07\x07\x08')
#
# Use this to convert a byte directly into a string
#
bits = ['00000000', '00000001', '00000010', '00000011', '00000100', '00000101', '00000110', '00000111',
        '00001000', '00001001', '00001010', '00001011', '00001100', '00001101', '00001110', '00001111',
        '00010000', '00010001', '00010010', '00010011', '00010100', '00010101', '00010110', '00010111',
        '00011000', '00011001', '00011010', '00011011', '00011100', '00011101', '00011110', '00011111',
        '00100000', '00100001', '00100010', '00100011', '00100100', '00100101', '00100110', '00100111',
        '00101000', '00101001', '00101010', '00101011', '00101100', '00101101', '00101110', '00101111',
        '00110000', '00110001', '00110010', '00110011', '00110100', '00110101', '00110110', '00110111',
        '00111000', '00111001', '00111010', '00111011', '00111100', '00111101', '00111110', '00111111',
        '01000000', '01000001', '01000010', '01000011', '01000100', '01000101', '01000110', '01000111',
        '01001000', '01001001', '01001010', '01001011', '01001100', '01001101', '01001110', '01001111',
        '01010000', '01010001', '01010010', '01010011', '01010100', '01010101', '01010110', '01010111',
        '01011000', '01011001', '01011010', '01011011', '01011100', '01011101', '01011110', '01011111',
        '01100000', '01100001', '01100010', '01100011', '01100100', '01100101', '01100110', '01100111',
        '01101000', '01101001', '01101010', '01101011', '01101100', '01101101', '01101110', '01101111',
        '01110000', '01110001', '01110010', '01110011', '01110100', '01110101', '01110110', '01110111',
        '01111000', '01111001', '01111010', '01111011', '01111100', '01111101', '01111110', '01111111',
        '10000000', '10000001', '10000010', '10000011', '10000100', '10000101', '10000110', '10000111',
        '10001000', '10001001', '10001010', '10001011', '10001100', '10001101', '10001110', '10001111',
        '10010000', '10010001', '10010010', '10010011', '10010100', '10010101', '10010110', '10010111',
        '10011000', '10011001', '10011010', '10011011', '10011100', '10011101', '10011110', '10011111',
        '10100000', '10100001', '10100010', '10100011', '10100100', '10100101', '10100110', '10100111',
        '10101000', '10101001', '10101010', '10101011', '10101100', '10101101', '10101110', '10101111',
        '10110000', '10110001', '10110010', '10110011', '10110100', '10110101', '10110110', '10110111',
        '10111000', '10111001', '10111010', '10111011', '10111100', '10111101', '10111110', '10111111',
        '11000000', '11000001', '11000010', '11000011', '11000100', '11000101', '11000110', '11000111',
        '11001000', '11001001', '11001010', '11001011', '11001100', '11001101', '11001110', '11001111',
        '11010000', '11010001', '11010010', '11010011', '11010100', '11010101', '11010110', '11010111',
        '11011000', '11011001', '11011010', '11011011', '11011100', '11011101', '11011110', '11011111',
        '11100000', '11100001', '11100010', '11100011', '11100100', '11100101', '11100110', '11100111',
        '11101000', '11101001', '11101010', '11101011', '11101100', '11101101', '11101110', '11101111',
        '11110000', '11110001', '11110010', '11110011', '11110100', '11110101', '11110110', '11110111',
        '11111000', '11111001', '11111010', '11111011', '11111100', '11111101', '11111110', '11111111' ]
#
# Use this to count the number of zeros in a byte.
# This does not verify that this is a byte with values between 0 and 255.
# len('{0:08b}'.format(i).replace("1", ""))
#
def zeroes_in_a_byte(b):
    return 8-ONE_COUNTS[b]
#
# Use this to count the number of ones in a byte.
# This does not verify that this is a byte with values between 0 and 255.
#
# This will work for all integers but will take longer to run!
# len(bin(i).replace("0b", "").replace("0", ""))
#
def ones_in_a_byte(one_byte):
    return ONE_COUNTS[one_byte]
#
# Return a byte as a binary string. This should be faster than this code:
# '{0:08b}'.format(i)
#
def byte_to_bits(one_byte):
    return bits[one_byte]
#
# How do I know if something is random?
# https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-22r1a.pdf
#
# I pulled Python code that implements the 15 tests for a random sequence as
# outlined in in the NIST document mentioned above.
# Python code from here: https://github.com/StuartGordonReid/r4nd0m
# Pull the code using: git clone https://github.com/StuartGordonReid/r4nd0m.git
#
# The code author has a write-up here:
#
# http://www.turingfinance.com/hacking-the-random-walk-hypothesis/
#
#
# This included code does NOT contain a copyright or license
# I thought I had seen that this was all under the MIT license,
# but I cannot find it now that I am looking for it.
#
# There is more code here than is needed, but I did not want
# to take the time to remove unused code. Some of these parts will
# probably fail because I did not copy all of the code over.
#
class Colors:
    """
    Just used to make the standard-out a little bit less ugly
    """
    Pass, Fail, End     = '\033[92m', '\033[91m', '\033[0m'
    Bold, Info, Italics = '\033[1m', '\033[94m', '\x1B[3m'
class RandomnessTester:
    """Test binary data for randomness"""
    def __init__(self, bin, real_data=False, start_year=00, end_year=00):
        """
        Initializes a RandomnessTester object. This object contains the NIST cryptographic tests
        for randomness.
        These tests only work on binary strings. The input data (bin) is a BinaryFrame object.
        A BinaryFrame object is simply a dictionary of lists containing binary strings. Each
        entry in the dictionary is an independent data set, and each binary string in the list
        represents a different time period. For example,
        bin.data = {    "JSE",      [010101010, 0111101111, 10101010000],
                        "S&P500",   [000000000, 1111111111, 01010101010] ... }
        Each binary string in the list is a sample. These samples are EACH fed through all of
        the NIST tests and their respective p-values are calculated. If ~96% of samples pass,
        the data set passes the randomness test(s) e.g. For more information see:
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        param bin: this is a "BinaryFrame" object which is a conversion of a
        pandas DataFrame into a binary dictionary
        """
        self.bin = bin
        self.real_data = real_data
        self.start_year = start_year
        self.end_year = end_year
        self.epsilon = 0.00001
        self.confidence_level = 0.005
    def get_string(self, p_val):
        """
        This method returns a string based on the p-value
        :param p_val: the p-value generated by a given test
        :return: a string for outputting to the console
        """
        if p_val >= 0:
            if p_val < self.confidence_level:
                return Colors.Fail + f"{p_val:.5f}" + "\t" + Colors.End
            else:
                return Colors.Pass + f"{p_val:.5f}" + "\t" + Colors.End
        else:
            return f"{p_val:.4f}" + "\t" + Colors.End
    def get_aggregate_pval(self, pvals):
        """
        This method applies a chi-squared test on a series of p-values to check their uniformity.
        :param pvals: the list of p-values for a given data set across all the NIST tests
        :return: the aggregate p-value based on the chi-squared test for the p values.
        """
        bin_counts = numpy.zeros(10)
        for p in pvals:
            pos = min(int(math.floor(p * 10)), 9)
            bin_counts[pos] += 1
        chi_squared = 0
        expected_count = len(pvals) / 10
        for bin_count in bin_counts:
            chi_squared += pow(bin_count - expected_count, 2.0) / expected_count
        return spc.gammaincc(9.0 / 2.0, chi_squared / 2.0)
    def get_aggregate_pass(self, pvals):
        """
        This method determines if a data set passed when you look at all the p-values associates
        with each of the binary strings associated with that data set. If the data set passes
        on ~96% of the samples it passes overall.
        :param pvals: the list of p-values for a given data set across all the NIST tests
        :return: the proportion of samples which passed their tests.
        """
        npvals = numpy.array(pvals)
        return (npvals > self.confidence_level).sum() / len(pvals)
    def print_dates(self, num_blocks):
        if self.real_data:
            filler = "".zfill(68)
            string_out = filler.replace("0", " ")
            step = math.floor((self.end_year - self.start_year) / num_blocks)
            dates = numpy.arange(start=self.start_year, stop=self.end_year, step=step)
            for i in range(num_blocks):
                start_string = "~" + str(int(dates[i]))
                string_out += start_string + "\t"
            print(string_out)
    #
    # Get a list of the test names.
    # If the "numbers" are returned, this indicates the test number as defined in the NIST document.
    #
    def get_test_names(self, pre_str:str="", post_str:str="", include_number=True):
        numbers    = ["01. ",
                      "02. ",
                      "03. ",
                      "04. ",
                      "05. ",
                      "06. ",
                      "07. ",
                      "08. ",
                      "09. ",
                      "10. ",
                      "11. ",
                      "11. ",
                      "12. ",
                      "13. ",
                      "13. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "14. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. ",
                      "15. "]
        test_names = ["Monobit Test",
                      "Block Frequency Test",
                      "Independent Runs Test",
                      "Longest Runs Test",
                      "Matrix Rank Test",
                      "Spectral Test",
                      "Non Overlapping Patterns Test",
                      "Overlapping Patterns Test",
                      "Universal Test",
                      "Linear Complexity Test",
                      "Serial Test (p01)",
                      "Serial Test (p02)",
                      "Approximate Entropy Test",
                      "Cumulative Sums Test (Forward)",
                      "Cumulative Sums Test (Backward)",
                      "Random Excursions Test (p01)",
                      "Random Excursions Test (p02)",
                      "Random Excursions Test (p03)",
                      "Random Excursions Test (p04)",
                      "Random Excursions Test (p05)",
                      "Random Excursions Test (906)",
                      "Random Excursions Test (p07)",
                      "Random Excursions Test (p08)",
                      "Random Excursions Variant Test (p01)",
                      "Random Excursions Variant Test (p02)",
                      "Random Excursions Variant Test (p03)",
                      "Random Excursions Variant Test (p04)",
                      "Random Excursions Variant Test (p05)",
                      "Random Excursions Variant Test (p06)",
                      "Random Excursions Variant Test (p07)",
                      "Random Excursions Variant Test (p08)",
                      "Random Excursions Variant Test (p09)",
                      "Random Excursions Variant Test (p10)",
                      "Random Excursions Variant Test (p11)",
                      "Random Excursions Variant Test (p12)",
                      "Random Excursions Variant Test (p13)",
                      "Random Excursions Variant Test (p14)",
                      "Random Excursions Variant Test (p15)",
                      "Random Excursions Variant Test (p16)",
                      "Random Excursions Variant Test (p17)",
                      "Random Excursions Variant Test (p18)"]
        return_names = []
        for i in range(len(test_names)):
            number = numbers[i] if include_number else ""
            return_names.append(pre_str + number + test_names[i] + post_str)
        return return_names
    #
    # Generate random bytes and run the tests against the generated bytes.
    # print_results - If True, print the test results for all tests run.
    # num_bytes     - Number bytes to generate. Most tests pass with values like 128, 256, or 512.
    #                 A value of 1048576 caused some of the tests to fail for this random number
    #                 generator.
    # block_size    - Block size to use. This is related to bits, not bytes.
    # matrix_size   - Suggested matrix size.
    #
    # Returns       - Percentage of tests passed ignoring tests skipped because the data was not
    #                 an appropriate size.
    #
    def run_test_against_random_bits(self, print_results, num_bytes=128,
                                     block_size=128, matrix_size=32):
        #
        # Generate a random number to test things.
        #
        ba = bytearray(os.urandom(num_bytes))
        return self.test_byte_array(print_results, ba, block_size, matrix_size)
    #
    # Run the tests against the byte array
    # Returns       - Percentage of tests passed ignoring tests skipped because the data
    #                 was not an appropriate size.
    #
    # print_results - If True, print the test results for all tests run.
    # ba            - Byte array to test. This is converted into a string of bits
    #                 (ones and zeros).
    # block_size    - Block size to use. This is related to bits, not bytes.
    # matrix_size   - Suggested matrix size.
    #
    def test_byte_array(self, print_results, ba, block_size=128, matrix_size=32):
        #
        # Now turn it into a string of ones and zeros, which is how this code works.
        # Not the most efficient way to deal with it, but the code is already written
        # so just use it.
        #
        str_data = ''
        for one_byte in ba:
            str_data = str_data + bits[one_byte]
        return self.test_bit_string(print_results, str_data, block_size, matrix_size)
    #
    # Probably should have used the CSV writter from the start, but the code was already written.
    #
    def csv_safe_string(self,string):
        outstream = io.StringIO()
        cw = csv.writer(outstream)
        cw.writerow([string])
        s = outstream.getvalue().rstrip()
        if s.startswith('"'):
            s = s[1:]
        if s.endswith('"'):
            s = s[:-1]
        return s
    #
    # The CSV header as I expect to print it.
    #
    def get_csv_header(self):
        test_names = self.get_test_names()

        csv_header = '"Index", "Percent", "Num Passed", "Num Skipped", "Num Failed", "Confidence Level", "' + \
                     '", "'.join(test_names) + '", "File", "Source IP", "Dest IP", "Data Len", "Data"'
        return csv_header
    #
    # Run the tests against the byte array
    # Returns       - Percentage of tests passed ignoring tests skipped because
    #                 the data was not an appropriate size.
    #
    # print_results - If True, print the test results for all tests run.
    # str_data      - String of bits (ones and zeros) against which to test.
    # block_size    - Block size to use. This is related to bits, not bytes.
    # matrix_size   - Suggested matrix size.
    #
    # Returns two things:
    # percentage of tests passed
    # A string that represents the CSV line that is missing the index for the packet
    # in the PCAP file and the PCAP file name.
    #
    def test_bit_string(self, print_results, str_data:str, block_size=128, matrix_size=32):
        test_names = self.get_test_names("\t")
        pvals = []
        pval_strings = []
        for i in range(len(test_names)):
            length = len(test_names[i])
            space = 45 - length
            filler = "".zfill(space)
            filler = filler.replace("0", " ")
            test_names[i] += filler
            pval_strings.append("")
        p_val = self.monobit(str_data)
        pval_strings[0] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.block_frequency(str_data, block_size=block_size)
        pval_strings[1] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.independent_runs(str_data)
        pval_strings[2] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.longest_runs(str_data)
        pval_strings[3] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.matrix_rank(str_data, matrix_size)
        pval_strings[4] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.spectral(str_data)
        pval_strings[5] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.non_overlapping_patterns(str_data, "11110000")
        pval_strings[6] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.overlapping_patterns(str_data, block_size=block_size)
        pval_strings[7] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.universal(str_data)
        pval_strings[8] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.linear_complexity(str_data, block_size=block_size)
        pval_strings[9] += self.get_string(p_val)
        pvals.append(p_val)
        p_val_one, p_val_two = self.serial(str_data, method="both")
        # The serial test can return two p-values add one
        pval_strings[10] += self.get_string(p_val_one)
        pvals.append(p_val_one)
        # The serial test can return two p-values add two
        pval_strings[11] += self.get_string(p_val_two)
        pvals.append(p_val_two)
        p_val = self.approximate_entropy(str_data)
        pval_strings[12] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.cumulative_sums(str_data, method="forward")
        pval_strings[13] += self.get_string(p_val)
        pvals.append(p_val)
        p_val = self.cumulative_sums(str_data, method="backward")
        pval_strings[14] += self.get_string(p_val)
        pvals.append(p_val)
        p_values = self.random_excursions(str_data)
        for j in range(15, 15 + 8):
            pvals.append(p_values[j - 15])
            pval_strings[j] += self.get_string(p_values[j - 15])
        p_values = self.random_excursions_variant(str_data)
        for j in range(23, 23 + 18):
            pvals.append(p_values[j - 23])
            pval_strings[j] += self.get_string(p_values[j - 23])
        # For each sample calculate the aggregate p_value and aggregate pass %
        # We have only one sample!
        #
        aggregate_pvals, aggregate_pass = [], []
        for j in range(len(pvals)):
            # print("pvals: " + str(pvals[j]) + " aggregate_pval: " + \
            #       str(self.get_aggregate_pval(pvals[j])) + " aggregate_pass: " +\
            #       str(self.get_aggregate_pass(pvals[j])))
            #
            # aggregate pval seems to be always the same because we have only a single sample:
            #
            aggregate_pvals.append(self.get_aggregate_pval([pvals[j]]))
            #
            # Check to see if the average pval is greater than self.confidence,
            # which defaults to 0.005.
            # This returns 1.0 or 0.0 for pass or fail.
            # Note that a skipped test is considered a fail.
            #
            aggregate_pass.append(self.get_aggregate_pass([pvals[j]]))
        tests_passed_this = 0
        tests_skipped_this = 0
        #
        # Build the csv string initially to hold a comma delimted list of the test values.
        #
        csv_string = ""
        for i in range(len(test_names)):
            pass_string = Colors.Bold + Colors.Fail + "FAIL!\t" + Colors.End
            # NIST documentation recommends 0.96 ... but also more samples
            if aggregate_pass[i] >= 0.90:
                pass_string = Colors.Bold + Colors.Pass + "PASS!\t" + Colors.End
                tests_passed_this += 1
            if (numpy.array(pvals[i]) == -1.0).sum() > 0:
                pass_string = Colors.Bold + "SKIP!\t" + Colors.End
            pval_string = Colors.Bold + Colors.Fail + "p=" + \
                          f'{aggregate_pvals[i]:.5f}' + "\t" + Colors.End
            if aggregate_pvals[i] > self.confidence_level:
                pval_string = Colors.Bold + Colors.Pass + "p=" + \
                              f'{aggregate_pvals[i]:.5f}' + "\t" + Colors.End
            if (numpy.array(pvals[i]) == -1.0).sum() > 0:
                pval_string = "p=SKIPPED\t"
                tests_skipped_this += 1
            csv_string = csv_string + ", " + f'{pvals[i]:.5f}'
            if print_results:
                print(test_names[i] + pass_string + pval_string + pval_strings[i])
        tests_failed_this = len(test_names) - tests_passed_this - tests_skipped_this
        if print_results:
            print("\n\npassed: " + str(tests_passed_this) + " failed: " + str(tests_failed_this) + \
                  " skipped: " + str(tests_skipped_this) + " self.confidence_level:" + \
                  str(self.confidence_level))
        passed_percentage = 0.0 if (len(test_names) - tests_skipped_this == 0) \
                                else tests_passed_this / (len(test_names) - tests_skipped_this)
        csv_string = str(passed_percentage) + ", " + str(tests_passed_this) + ", " + \
                     str(tests_skipped_this) + ", " + \
                     str(len(test_names) - tests_skipped_this - tests_passed_this) + ", " + \
                     str(self.confidence_level) + csv_string
        #print(csv_string)
        return passed_percentage, csv_string
    def run_test_suite(self, block_size, matrix_size):
        """
        This method runs all of the tests included in the NIST test suite for randomness
        :param block_size: the length of each block to look at for each bit string
        :param matrix_size: the size of the matrix to look at for each bit string
        """
        tests_passed = []
        # For each data set in self.bin
        for c in self.bin.columns:
            print(Colors.Bold + "\n\tRunning " + self.bin.method + \
                  " based tests on", c + Colors.End, "\n")
            test_names = ["\t01. Monobit Test",
                          "\t02. Block Frequency Test",
                          "\t03. Independent Runs Test",
                          "\t04. Longest Runs Test",
                          "\t05. Matrix Rank Test",
                          "\t06. Spectral Test",
                          "\t07. Non Overlapping Patterns Test",
                          "\t08. Overlapping Patterns Test",
                          "\t09. Universal Test",
                          "\t10. Linear Complexity Test",
                          "\t11. Serial Test (p01)",
                          "\t11. Serial Test (p02)",
                          "\t12. Approximate Entropy Test",
                          "\t13. Cumulative Sums Test (Forward)",
                          "\t13. Cumulative Sums Test (Backward)",
                          "\t14. Random Excursions Test (p01)",
                          "\t14. Random Excursions Test (p02)",
                          "\t14. Random Excursions Test (p03)",
                          "\t14. Random Excursions Test (p04)",
                          "\t14. Random Excursions Test (p05)",
                          "\t14. Random Excursions Test (906)",
                          "\t14. Random Excursions Test (p07)",
                          "\t14. Random Excursions Test (p08)",
                          "\t15. Random Excursions Variant Test (p01)",
                          "\t15. Random Excursions Variant Test (p02)",
                          "\t15. Random Excursions Variant Test (p03)",
                          "\t15. Random Excursions Variant Test (p04)",
                          "\t15. Random Excursions Variant Test (p05)",
                          "\t15. Random Excursions Variant Test (p06)",
                          "\t15. Random Excursions Variant Test (p07)",
                          "\t15. Random Excursions Variant Test (p08)",
                          "\t15. Random Excursions Variant Test (p09)",
                          "\t15. Random Excursions Variant Test (p10)",
                          "\t15. Random Excursions Variant Test (p11)",
                          "\t15. Random Excursions Variant Test (p12)",
                          "\t15. Random Excursions Variant Test (p13)",
                          "\t15. Random Excursions Variant Test (p14)",
                          "\t15. Random Excursions Variant Test (p15)",
                          "\t15. Random Excursions Variant Test (p16)",
                          "\t15. Random Excursions Variant Test (p17)",
                          "\t15. Random Excursions Variant Test (p18)"]
            pvals = []
            pval_strings = []
            for i in range(len(test_names)):
                length = len(test_names[i])
                space = 45 - length
                filler = "".zfill(space)
                filler = filler.replace("0", " ")
                test_names[i] += filler
                pvals.append([])
                pval_strings.append("")
            # Get the samples for the data set
            binary_strings = self.bin.bin_data[c]
            # Run each one of the tests and record the p_values
            for i in range(len(binary_strings)):
                passed_values, p_values, str_data = [], [], binary_strings[i]
                p_val = self.monobit(str_data)
                pval_strings[0] += self.get_string(p_val)
                pvals[0].append(p_val)
                p_val = self.block_frequency(str_data, block_size=block_size)
                pval_strings[1] += self.get_string(p_val)
                pvals[1].append(p_val)
                p_val = self.independent_runs(str_data)
                pval_strings[2] += self.get_string(p_val)
                pvals[2].append(p_val)
                p_val = self.longest_runs(str_data)
                pval_strings[3] += self.get_string(p_val)
                pvals[3].append(p_val)
                p_val = self.matrix_rank(str_data, matrix_size)
                pval_strings[4] += self.get_string(p_val)
                pvals[4].append(p_val)
                p_val = self.spectral(str_data)
                pval_strings[5] += self.get_string(p_val)
                pvals[5].append(p_val)
                p_val = self.non_overlapping_patterns(str_data, "11110000")
                pval_strings[6] += self.get_string(p_val)
                pvals[6].append(p_val)
                p_val = self.overlapping_patterns(str_data, block_size=block_size)
                pval_strings[7] += self.get_string(p_val)
                pvals[7].append(p_val)
                p_val = self.universal(str_data)
                pval_strings[8] += self.get_string(p_val)
                pvals[8].append(p_val)
                p_val = self.linear_complexity(str_data, block_size=block_size)
                pval_strings[9] += self.get_string(p_val)
                pvals[9].append(p_val)
                p_val_one, p_val_two = self.serial(str_data, method="both")
                # The serial test can return two p-values add one
                pval_strings[10] += self.get_string(p_val_one)
                pvals[10].append(p_val_one)
                # The serial test can return two p-values add two
                pval_strings[11] += self.get_string(p_val_two)
                pvals[11].append(p_val_two)
                p_val = self.approximate_entropy(str_data)
                pval_strings[12] += self.get_string(p_val)
                pvals[12].append(p_val)
                p_val = self.cumulative_sums(str_data, method="forward")
                pval_strings[13] += self.get_string(p_val)
                pvals[13].append(p_val)
                p_val = self.cumulative_sums(str_data, method="backward")
                pval_strings[14] += self.get_string(p_val)
                pvals[14].append(p_val)
                p_values = self.random_excursions(str_data)
                for j in range(15, 15 + 8):
                    pvals[j].append(p_values[j - 15])
                    pval_strings[j] += self.get_string(p_values[j - 15])
                p_values = self.random_excursions_variant(str_data)
                for j in range(23, 23 + 18):
                    pvals[j].append(p_values[j - 23])
                    pval_strings[j] += self.get_string(p_values[j - 23])
            # For each sample calculate the aggregate p_value and aggregate pass %
            aggregate_pvals, aggregate_pass = [], []
            for i in range(len(binary_strings)):
                for j in range(len(pvals)):
                    aggregate_pvals.append(self.get_aggregate_pval(pvals[j]))
                    aggregate_pass.append(self.get_aggregate_pass(pvals[j]))
            tests_passed_this = 0
            # Print the results to the console
            self.print_dates(len(binary_strings))
            for i in range(len(test_names)):
                pass_string = Colors.Bold + Colors.Fail + "FAIL!\t" + Colors.End
                # NIST documentation recommends 0.96 ... but also more samples
                if aggregate_pass[i] >= 0.90:
                    pass_string = Colors.Bold + Colors.Pass + "PASS!\t" + Colors.End
                    tests_passed_this += 1
                if (numpy.array(pvals[i]) == -1.0).sum() > 0:
                    pass_string = Colors.Bold + "SKIP!\t" + Colors.End
                pval_string = Colors.Bold + Colors.Fail + "p=" + \
                              f'{aggregate_pvals[i]:.5f}' + "\t" + Colors.End
                if aggregate_pvals[i] > self.confidence_level:
                    pval_string = Colors.Bold + Colors.Pass + "p=" + \
                                  f'{aggregate_pvals[i]:.5f}' + "\t" + Colors.End
                if (numpy.array(pvals[i]) == -1.0).sum() > 0:
                    pval_string = "p=SKIPPED\t"
                print(test_names[i] + pass_string + pval_string + pval_strings[i])
            tests_passed.append(tests_passed_this)
        return tests_passed
    def load_test_data(self, data_set_name):
        """
        This method is used to load in a test-data binary string.
        These data sets are included in the TestData directory
        :param data_set_name: the name of the test data set to load e.g. e.csv, pi.csv, etc.
        :return: a raw binary string of the data
        """
        try:
            raw_data = ""
            path = os.path.join(os.getcwd(), os.pardir, "TestData", data_set_name)
            with open(path, 'r+') as data_set_file:
                for line in data_set_file:
                    raw_data += line.replace("\n", "").replace("\t", "").replace(" ", "")
            return raw_data
        except FileNotFoundError:
            path = os.path.join(os.getcwd(), os.pardir, "TestData", data_set_name)
            print("File not found", path, "exiting")
            exit(0)
    def generic_checker(self, test_name, expected, function, actual_out=None):
        """
        This is a generic method for checking the outputs from one of the tests against
        known outputs to ensure that the
        test if acting as expected. Essentially it is a unit tester.
        :param test_name: the name of the test being checked
        :param expected: a list of expected p-values
        :param function: a reference to the function being checked
        """
        # Compute the output using the function
        print("\n\t", Colors.Bold + test_name + Colors.End)
        if "Complexity" in test_name or "Matrix" in test_name:
            print("\t", "This may take a while please be patient.")
        data_sets = ["pi", "e", "sqrt2", "sqrt3"]
        if actual_out is None:
            for i in range(len(data_sets)):
                p_val = function(self.load_test_data(data_sets[i])[:1000000])
                data_set_label = "".zfill(10 - len(data_sets[i])).replace("0", " ")
                if abs(p_val - expected[i]) < self.epsilon:
                    print("\t", Colors.Pass + data_sets[i], data_set_label,
                          "\tp expected = ", expected[i],
                          "\tp computed =", f'{p_val:.6f}' + Colors.End)
                else:
                    print("\t", Colors.Fail + data_sets[i], data_set_label,
                          "\tp expected = ", expected[i],
                          "\tp computed =", f'{p_val:.6f}' + Colors.End)
        # Output has already been supplied
        else:
            i = 0
            for p_val in actual_out:
                data_set_label = "".zfill(10 - len(data_sets[i])).replace("0", " ")
                if abs(p_val - expected[i]) < self.epsilon:
                    print("\t", Colors.Pass + data_sets[i], data_set_label,
                          "\tp expected = ", expected[i],
                          "\tp computed =", f'{p_val:.6f}' + Colors.End)
                else:
                    print("\t", Colors.Fail + data_sets[i], data_set_label,
                          "\tp expected = ", expected[i],
                          "\tp computed =", f'{p_val:.6f}' + Colors.End)
                i += 1
    def test_randomness_tester(self):
        """
        This method calls the method calls each one of the checks of the
        randomness tests contained in this class
        """
        self.monobit_check()
        self.block_frequency_check()
        self.independent_runs_check()
        self.longest_runs_check()
        self.spectral_check()
        self.non_overlapping_patterns_check()
        self.overlapping_patterns_check()
        self.universal_check()
        self.serial_check()
        self.approximate_entropy_check()
        self.cumulative_sums_check()
        self.random_excursions_check()
        self.random_excursions_variant_check()
        # These checks are slow
        self.matrix_rank_check()
        self.linear_complexity_check()
    def count_zeros_and_ones(self, bin_data: str):
        """
        This is just a simple method for counting zeros and ones
        :param bin_data: the data from which to count zeros and ones
        :return: nothing.
        """
        ones, zeros = 0, 0
        # If the char is 0 minus 1, else add 1
        for char in bin_data:
            if char == '0':
                zeros += 1
            else:
                ones += 1
        print("\t", Colors.Italics + "Count 1 =", ones, "Count 0 =", zeros, Colors.End)
    def monobit(self, bin_data: str):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the proportion of zeros and ones for the entire sequence.
        The purpose of this test is to determine whether the number of ones and zeros in a
        sequence are approximately the same as would be expected for a truly random sequence.
        This test assesses the closeness of the fraction of ones to 1/2, that is the number
        of ones and zeros ina  sequence should be about the same.
        All subsequent tests depend on this test.
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        count = 0
        num_ones = 0
        num_zeros = 0
        # If the char is 0 minus 1, else add 1
        for char in bin_data:
            if char == '0':
                count -= 1
                num_zeros += 1
            else:
                count += 1
                num_ones += 1
        # Calculate the p value
        sobs = count / math.sqrt(len(bin_data))
        p_val = spc.erfc(math.fabs(sobs) / math.sqrt(2))
        return p_val
    def monobit_check(self):
        """
        This is a test method for the monobit test method based on the example
        in the NIST documentation
        """
        expected = [0.578211, 0.953749, 0.811881, 0.610051]
        self.generic_checker("Testing Monobit Test", expected, self.monobit)
    def block_frequency(self, bin_data: str, block_size=128):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this tests is the proportion of ones within M-bit blocks.
        The purpose of this tests is to determine whether the frequency of ones
        in an M-bit block is approximately M/2, as would be expected under an assumption
        of randomness. For block size M=1, this test degenerates to the monobit frequency test.
        :param bin_data: a binary string
        :return: the p-value from the test
        :param block_size: the size of the blocks that the binary sequence is partitioned into
        """
        # Work out the number of blocks, discard the remainder
        num_blocks = math.floor(len(bin_data) / block_size)
        block_start, block_end = 0, block_size
        # Keep track of the proportion of ones per block
        proportion_sum = 0.0
        for i in range(num_blocks):
            # Slice the binary string into a block
            block_data = bin_data[block_start:block_end]
            # Keep track of the number of ones
            ones_count = 0
            for char in block_data:
                if char == '1':
                    ones_count += 1
            pi = ones_count / block_size
            proportion_sum += pow(pi - 0.5, 2.0)
            # Update the slice locations
            block_start += block_size
            block_end += block_size
        # Calculate the p-value
        chi_squared = 4.0 * block_size * proportion_sum
        p_val = spc.gammaincc(num_blocks / 2, chi_squared / 2)
        return p_val
    def block_frequency_check(self):
        """
        This is a test method for the block frequency test method based on the example
        in the NIST documentation
        """
        expected = [0.380615, 0.211072, 0.833222, 0.473961]
        self.generic_checker("Testing Block Frequency Test", expected, self.block_frequency)
    def independent_runs(self, bin_data: str):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this tests if the total number of runs in the sequences,
        where a run is an uninterrupted sequence of identical bits.
        A run of length k consists of k identical bits and is bounded before and
        after with a bit of the opposite value. The purpose of the runs tests is
        to determine whether the number of runs of ones and zeros of various lengths
        is as expected for a random sequence. In particular, this tests determines
        whether the oscillation between zeros and ones is either too fast or too slow.
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        ones_count, n = 0, len(bin_data)
        for char in bin_data:
            if char == '1':
                ones_count += 1
        p, vobs = float(ones_count / n), 1
        tau = 2 / math.sqrt(len(bin_data))
        if abs(p - 0.5) > tau:
            return 0.0
        else:
            for i in range(1, n):
                if bin_data[i] != bin_data[i - 1]:
                    vobs += 1
            # expected_runs = 1 + 2 * (n - 1) * 0.5 * 0.5
            # print("\t", Colors.Italics + "Observed runs =", vobs,
            #       "Expected runs", expected_runs, Colors.End)
            num = abs(vobs - 2.0 * n * p * (1.0 - p))
            den = 2.0 * math.sqrt(2.0 * n) * p * (1.0 - p)
            p_val = spc.erfc(float(num / den))
            return p_val
    def independent_runs_check(self):
        """
        This is a test method for the runs test method based on the example
        in the NIST documentation
        """
        expected = [0.419268, 0.561917, 0.313427, 0.261123]
        self.generic_checker("Testing Independent Runs Test", expected, self.independent_runs)
    def longest_runs(self, bin_data: str):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of the tests is the longest run of ones within M-bit blocks.
        The purpose of this tests is to determine whether the length of the longest
        run of ones within the tested sequences is consistent with the length of the
        longest run of ones that would be expected in a random sequence.
        Note that an irregularity in the expected length of the longest
        run of ones implies that there is also an irregularity ub tge expected length
        of the long est run of zeroes. Therefore, only one test is necessary
        for this statistical tests of randomness
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        if len(bin_data) < 128:
            print("\t", "Not enough data to run test!")
            return -1.0
        elif len(bin_data) < 6272:
            k, m = 3, 8
            v_values = [1, 2, 3, 4]
            pik_values = [0.21484375, 0.3671875, 0.23046875, 0.1875]
        elif len(bin_data) < 75000:
            k, m = 5, 128
            v_values = [4, 5, 6, 7, 8, 9]
            pik_values = [0.1174035788, 0.242955959, 0.249363483, 0.17517706,
                          0.102701071, 0.112398847]
        else:
            k, m = 6, 10000
            v_values = [10, 11, 12, 13, 14, 15, 16]
            pik_values = [0.0882, 0.2092, 0.2483, 0.1933, 0.1208, 0.0675, 0.0727]
        # Work out the number of blocks, discard the remainder
        # pik = [0.2148, 0.3672, 0.2305, 0.1875]
        num_blocks = math.floor(len(bin_data) / m)
        frequencies = numpy.zeros(k + 1)
        block_start, block_end = 0, m
        for i in range(num_blocks):
            # Slice the binary string into a block
            block_data = bin_data[block_start:block_end]
            # Keep track of the number of ones
            max_run_count, run_count = 0, 0
            for j in range(0, m):
                if block_data[j] == '1':
                    run_count += 1
                    max_run_count = max(max_run_count, run_count)
                else:
                    max_run_count = max(max_run_count, run_count)
                    run_count = 0
            max_run_count = max(max_run_count, run_count)
            if max_run_count < v_values[0]:
                frequencies[0] += 1
            for j in range(k):
                if max_run_count == v_values[j]:
                    frequencies[j] += 1
            if max_run_count > v_values[k - 1]:
                frequencies[k] += 1
            block_start += m
            block_end += m
        # print(frequencies)
        chi_squared = 0
        for i in range(len(frequencies)):
            chi_squared += (pow(frequencies[i] - (num_blocks * pik_values[i]), 2.0)) / \
                           (num_blocks * pik_values[i])
        p_val = spc.gammaincc(float(k / 2), float(chi_squared / 2))
        return p_val
    def longest_runs_check(self):
        """
        This is a test method for the longest run test method based on the example
        in the NIST documentation
        """
        expected = [0.024390, 0.718945, 0.012117, 0.446726]
        self.generic_checker("Testing Longest Runs Test", expected, self.longest_runs)
    def matrix_rank(self, bin_data: str, matrix_size=32):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of the test is the rank of disjoint sub-matrices of the entire sequence.
        The purpose of this test is to check for linear dependence among fixed length
        sub strings of the original sequence. Note that this test
        also appears in the DIEHARD battery of tests.
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        shape = (matrix_size, matrix_size)
        bin_data_len = len(bin_data)
        block_size = int(matrix_size * matrix_size)
        num_m = math.floor(bin_data_len / (matrix_size * matrix_size))
        block_start, block_end = 0, block_size
        # print(q, bin_data_len, num_m, block_size)
        if num_m > 0:
            max_ranks = [0, 0, 0]
            for im in range(num_m):
                block_data = bin_data[block_start:block_end]
                block = numpy.zeros(len(block_data))
                for i in range(len(block_data)):
                    if block_data[i] == '1':
                        block[i] = 1.0
                m_shape = block.reshape(shape)
                ranker = BinaryMatrix(m_shape, matrix_size, matrix_size)
                rank = ranker.compute_rank()
                # print(rank)
                if rank == matrix_size:
                    max_ranks[0] += 1
                elif rank == (matrix_size - 1):
                    max_ranks[1] += 1
                else:
                    max_ranks[2] += 1
                # Update index trackers
                block_start += block_size
                block_end += block_size
            piks = [1.0, 0.0, 0.0]
            for x in range(1, 50):
                piks[0] *= 1 - (1.0 / (2 ** x))
            piks[1] = 2 * piks[0]
            piks[2] = 1 - piks[0] - piks[1]
            chi = 0.0
            for i in range(len(piks)):
                chi += pow((max_ranks[i] - piks[i] * num_m), 2.0) / (piks[i] * num_m)
            p_val = math.exp(-chi / 2)
            return p_val
        else:
            return -1.0
    def matrix_rank_check(self):
        """
        This is a test method for the binary matrix rank test based on the example
        in the NIST documentation
        """
        expected = [0.083553, 0.306156, 0.823810, 0.314498]
        self.generic_checker("Testing Matrix Rank Test", expected, self.matrix_rank)
    def spectral(self, bin_data: str):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the peak heights in the Discrete Fourier Transform
        of the sequence. The purpose of this test is to detect periodic features
        (i.e., repetitive patterns that are near each other) in the tested sequence
        that would indicate a deviation from the assumption of randomness.
        The intention is to detect whether
        the number of peaks exceeding the 95 % threshold is significantly different than 5 %.
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        bin_data_len = len(bin_data)
        plus_minus_one = []
        for char in bin_data:
            if char == '0':
                plus_minus_one.append(-1)
            elif char == '1':
                plus_minus_one.append(1)
        # Product discrete fourier transform of plus minus one
        s_fft = sff.fft(plus_minus_one)
        modulus = numpy.abs(s_fft[0:bin_data_len // 2])
        tau = numpy.sqrt(numpy.log(1 / 0.05) * bin_data_len)
        # Theoretical number of peaks
        count_n0 = 0.95 * (bin_data_len / 2)
        # Count the number of actual peaks m > T
        count_n1 = len(numpy.where(modulus < tau)[0])
        # Calculate d and return the p value statistic
        intermediate_d_value = (count_n1 - count_n0) / numpy.sqrt(bin_data_len * 0.95 * 0.05 / 4)
        p_val = spc.erfc(abs(intermediate_d_value) / numpy.sqrt(2))
        return p_val
    def spectral_check(self):
        """
        This is a test method for the spectral test based on the example
        in the NIST documentation
        """
        expected = [0.010186, 0.847187, 0.581909, 0.776046]
        self.generic_checker("Check Spectral Test", expected, self.spectral)
    def non_overlapping_patterns(self, bin_data: str, pattern="000000001", num_blocks=8):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the number of occurrences of pre-specified
        target strings. The purpose of this test is to detect generators that
        produce too many occurrences of a given non-periodic (aperiodic) pattern.
        For this test and for the Overlapping Template Matching test of Section 2.8,
        an m-bit window is used to search for a specific m-bit pattern.
        If the pattern is not found, the window slides one bit position.
        If the pattern is found, the window is reset to the bit after the
        found pattern, and the search resumes.
        :param bin_data: a binary string
        :param pattern: the pattern to match to
        :return: the p-value from the test
        """
        bin_data_len = len(bin_data)
        pattern_size = len(pattern)
        block_size = math.floor(bin_data_len / num_blocks)
        pattern_counts = numpy.zeros(num_blocks)
        # For each block in the data
        for i in range(num_blocks):
            block_start = i * block_size
            block_end = block_start + block_size
            block_data = bin_data[block_start:block_end]
            # Count the number of pattern hits
            j = 0
            while j < block_size:
                sub_block = block_data[j:j + pattern_size]
                if sub_block == pattern:
                    pattern_counts[i] += 1
                    j += pattern_size
                else:
                    j += 1
        # Calculate the theoretical mean and variance
        mean = (block_size - pattern_size + 1) / pow(2, pattern_size)
        var = block_size * ((1 / pow(2, pattern_size)) - (((2 * pattern_size) - 1) /
            (pow(2, pattern_size * 2))))
        # Calculate the Chi Squared statistic for these pattern matches
        chi_squared = 0
        for i in range(num_blocks):
            chi_squared += pow(pattern_counts[i] - mean, 2.0) / var
        # Calculate and return the p value statistic
        p_val = spc.gammaincc(num_blocks / 2, chi_squared / 2)
        return p_val
    def non_overlapping_patterns_check(self):
        """
        This is a test method for the non overlapping patterns test based on the example
        in the NIST documentation
        """
        expected = [0.165757, 0.078790, 0.569461, 0.532235]
        self.generic_checker("Check Non Overlapping Patterns Test", expected,
            self.non_overlapping_patterns)
    def overlapping_patterns(self, bin_data: str, pattern_size=9, block_size=1032):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of the Overlapping Template Matching test is the number of
        occurrences of pre-specified target strings. Both this test and the
        Non-overlapping Template Matching test of Section 2.7 use an m-bit
        window to search for a specific m-bit pattern. As with the test in Section 2.7,
        if the pattern is not found, the window slides one bit position.
        The difference between this test and the test in Section 2.7 is that
        when the pattern is found, the window slides only one bit before resuming the search.
        :param bin_data: a binary string
        :param pattern_size: the length of the pattern
        :return: the p-value from the test
        """
        bin_data_len = len(bin_data)
        pattern = ""
        for i in range(pattern_size):
            pattern += "1"
        num_blocks = math.floor(bin_data_len / block_size)
        lambda_val = float(block_size - pattern_size + 1) / pow(2, pattern_size)
        eta = lambda_val / 2.0
        piks = [self.get_prob(i, eta) for i in range(5)]
        diff = float(numpy.array(piks).sum())
        piks.append(1.0 - diff)
        pattern_counts = numpy.zeros(6)
        for i in range(num_blocks):
            block_start = i * block_size
            block_end = block_start + block_size
            block_data = bin_data[block_start:block_end]
            # Count the number of pattern hits
            pattern_count = 0
            j = 0
            while j < block_size:
                sub_block = block_data[j:j + pattern_size]
                if sub_block == pattern:
                    pattern_count += 1
                j += 1
            if pattern_count <= 4:
                pattern_counts[pattern_count] += 1
            else:
                pattern_counts[5] += 1
        chi_squared = 0.0
        for i in range(len(pattern_counts)):
            chi_squared += pow(pattern_counts[i] - num_blocks * piks[i], 2.0) / \
                           (num_blocks * piks[i])
        return spc.gammaincc(5.0 / 2.0, chi_squared / 2.0)
    def get_prob(self, u, x):
        out = 1.0 * numpy.exp(-x)
        if u != 0:
            out = 1.0 * x * numpy.exp(2 * -x) * (2 ** -u) * spc.hyp1f1(u + 1, 2, x)
        return out
    def overlapping_patterns_check(self):
        """
        This is a test method for the non overlapping patterns test based on the example
        in the NIST documentation
        """
        expected = [0.296897, 0.110434, 0.791982, 0.082716]
        self.generic_checker("Check Overlapping Patterns Test", expected, self.overlapping_patterns)
    def universal(self, bin_data: str):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the number of bits between matching patterns
        (a measure that is related to the length of a compressed sequence).
        The purpose of the test is to detect whether or not the sequence can be
        significantly compressed without loss of information.
        A significantly compressible sequence is considered to be non-random.
        :param bin_data: a binary string
        :return: the p-value from the test
        """
        # The below table is less relevant for us traders and markets than it is for security people
        bin_data_len = len(bin_data)
        pattern_size = 5
        if bin_data_len >= 387840:
            pattern_size = 6
        if bin_data_len >= 904960:
            pattern_size = 7
        if bin_data_len >= 2068480:
            pattern_size = 8
        if bin_data_len >= 4654080:
            pattern_size = 9
        if bin_data_len >= 10342400:
            pattern_size = 10
        if bin_data_len >= 22753280:
            pattern_size = 11
        if bin_data_len >= 49643520:
            pattern_size = 12
        if bin_data_len >= 107560960:
            pattern_size = 13
        if bin_data_len >= 231669760:
            pattern_size = 14
        if bin_data_len >= 496435200:
            pattern_size = 15
        if bin_data_len >= 1059061760:
            pattern_size = 16
        if 5 < pattern_size < 16:
            # Create the biggest binary string of length pattern_size
            ones = ""
            for i in range(pattern_size):
                ones += "1"
            # How long the state list should be
            num_ints = int(ones, 2)
            vobs = numpy.zeros(num_ints + 1)
            # Keeps track of the blocks, and whether were are initializing or summing
            num_blocks = math.floor(bin_data_len / pattern_size)
            init_bits = 10 * pow(2, pattern_size)
            test_bits = num_blocks - init_bits
            # These are the expected values assuming randomness (uniform)
            c = 0.7 - 0.8 / pattern_size + (4 + 32 / pattern_size) * \
                pow(test_bits, -3 / pattern_size) / 15
            variance = [0, 0, 0, 0, 0, 0, 2.954, 3.125, 3.238, 3.311, 3.356, 3.384, 3.401,
                        3.410, 3.416, 3.419, 3.421]
            expected = [0, 0, 0, 0, 0, 0, 5.2177052, 6.1962507, 7.1836656, 8.1764248, 9.1723243,
                        10.170032, 11.168765, 12.168070, 13.167693, 14.167488, 15.167379]
            sigma = c * math.sqrt(variance[pattern_size] / test_bits)
            cumsum = 0.0
            for i in range(num_blocks):
                block_start = i * pattern_size
                block_end = block_start + pattern_size
                block_data = bin_data[block_start: block_end]
                # Work out what state we are in
                int_rep = int(block_data, 2)
                # Initialize the state list
                if i < init_bits:
                    vobs[int_rep] = i + 1
                else:
                    initial = vobs[int_rep]
                    vobs[int_rep] = i + 1
                    cumsum += math.log(i - initial + 1, 2)
            # Calculate the statistic
            phi = float(cumsum / test_bits)
            stat = abs(phi - expected[pattern_size]) / (float(math.sqrt(2)) * sigma)
            p_val = spc.erfc(stat)
            return p_val
        else:
            return -1.0
    def universal_check(self):
        """
        This is a test method for the universal test based on the examples
        in the NIST documentation
        """
        expected = [0.669012, 0.282568, 0.130805, 0.165981]
        self.generic_checker("Check Universal Test", expected, self.universal)
    def linear_complexity(self, bin_data: str, block_size=500):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the length of a linear feedback shift register (LFSR).
        The purpose of this test is to determine whether or not the sequence is complex
        enough to be considered random. Random sequences are characterized by longer
        LFSRs. An LFSR that is too short implies non-randomness.
        :param bin_data: a binary string
        :param block_size: the size of the blocks to divide bin_data into.
        Recommended block_size >= 500
        :return:
        """
        dof = 6
        piks = [0.01047, 0.03125, 0.125, 0.5, 0.25, 0.0625, 0.020833]
        t2 = (block_size / 3.0 + 2.0 / 9) / 2 ** block_size
        mean = 0.5 * block_size + (1.0 / 36) * (9 + (-1) ** (block_size + 1)) - t2
        num_blocks = int(len(bin_data) / block_size)
        if num_blocks > 1:
            block_end = block_size
            block_start = 0
            blocks = []
            for i in range(num_blocks):
                blocks.append(bin_data[block_start:block_end])
                block_start += block_size
                block_end += block_size
            complexities = []
            for block in blocks:
                complexities.append(self.berlekamp_massey_algorithm(block))
            t = ([-1.0 * (((-1) ** block_size) * (chunk - mean) + 2.0 / 9) for
                chunk in complexities])
            vg = numpy.histogram(t, bins=[-9999999999, -2.5, -1.5, -0.5, 0.5,
                1.5, 2.5, 9999999999])[0][::-1]
            im = ([((vg[ii] - num_blocks * piks[ii]) ** 2) /
                (num_blocks * piks[ii]) for ii in range(7)])
            chi_squared = 0.0
            for i in range(len(piks)):
                chi_squared += im[i]
            p_val = spc.gammaincc(dof / 2.0, chi_squared / 2.0)
            return p_val
        else:
            return -1.0
    def berlekamp_massey_algorithm(self, block_data: str):
        """
        An implementation of the Berlekamp Massey Algorithm. Taken from Wikipedia
        https://en.wikipedia.org/wiki/Berlekamp-Massey_algorithm
        The Berlekamp–Massey algorithm is an algorithm that will find the shortest
        linear feedback shift register (LFSR) for a given binary output sequence.
        The algorithm will also find the minimal polynomial of a linearly recurrent
        sequence in an arbitrary field. The field requirement means that the
        Berlekamp–Massey algorithm requires all
        non-zero elements to have a multiplicative inverse.
        :param block_data:
        :return:
        """
        block_data_len = len(block_data)
        c_matrix = numpy.zeros(block_data_len)
        b_matrix = numpy.zeros(block_data_len)
        c_matrix[0], b_matrix[0] = 1, 1
        l_index, m_index, data_index = 0, -1, 0
        int_data = [int(el) for el in block_data]
        while data_index < block_data_len:
            v_data = int_data[(data_index - l_index):data_index]
            v_data = v_data[::-1]
            cc_matrix = c_matrix[1:l_index + 1]
            d_rank = (int_data[data_index] + numpy.dot(v_data, cc_matrix)) % 2
            if d_rank == 1:
                temp = copy.copy(c_matrix)
                p_array = numpy.zeros(block_data_len)
                for j in range(0, l_index):
                    if b_matrix[j] == 1:
                        p_array[j + data_index - m_index] = 1
                c_matrix = (c_matrix + p_array) % 2
                if l_index <= 0.5 * data_index:
                    l_index = data_index + 1 - l_index
                    m_index = data_index
                    b_matrix = temp
            data_index += 1
        return l_index
    def linear_complexity_check(self):
        """
        This is a test method for the linear complexity test based on the examples
        in the NIST documentation
        """
        expected = [0.255475, 0.826335, 0.317127, 0.346469]
        self.generic_checker("Check Linear Complexity Test", expected, self.linear_complexity)
    def serial(self, bin_data: str, pattern_length=16, method="first"):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the frequency of all possible overlapping m-bit
        patterns across the entire sequence. The purpose of this test is to
        determine whether the number of occurrences of the 2m m-bit overlapping
        patterns is approximately the same as would be expected for a random sequence.
        Random sequences have uniformity; that is, every m-bit pattern has the same
        chance of appearing as every other m-bit pattern. Note that for m = 1,
        the Serial test is equivalent to the Frequency test of Section 2.1.
        :param bin_data: a binary string
        :param pattern_length: the length of the pattern (m)
        :return: the P value
        """
        bin_data_len = len(bin_data)
        # Add first m-1 bits to the end
        bin_data += bin_data[:pattern_length - 1:]
        # Get max length one patterns for m, m-1, m-2
        max_pattern = ''
        for i in range(pattern_length + 1):
            max_pattern += '1'
        # Keep track of each pattern's frequency (how often it appears)
        vobs_one = numpy.zeros(int(max_pattern[0:pattern_length:], 2) + 1)
        vobs_two = numpy.zeros(int(max_pattern[0:pattern_length - 1:], 2) + 1)
        vobs_thr = numpy.zeros(int(max_pattern[0:pattern_length - 2:], 2) + 1)
        for i in range(bin_data_len):
            # Work out what pattern is observed
            vobs_one[int(bin_data[i:i + pattern_length:], 2)] += 1
            vobs_two[int(bin_data[i:i + pattern_length - 1:], 2)] += 1
            vobs_thr[int(bin_data[i:i + pattern_length - 2:], 2)] += 1
        vobs = [vobs_one, vobs_two, vobs_thr]
        sums = numpy.zeros(3)
        for i in range(3):
            for j in range(len(vobs[i])):
                sums[i] += pow(vobs[i][j], 2)
            sums[i] = (sums[i] * pow(2, pattern_length - i) / bin_data_len) - bin_data_len
        # Calculate the test statistics and p values
        del1 = sums[0] - sums[1]
        del2 = sums[0] - 2.0 * sums[1] + sums[2]
        p_val_one = spc.gammaincc(pow(2, pattern_length - 1) / 2, del1 / 2.0)
        p_val_two = spc.gammaincc(pow(2, pattern_length - 2) / 2, del2 / 2.0)
        # For checking the outputs
        if method == "first":
            return p_val_one
        elif method == "both":
            return p_val_one, p_val_two
        else:
            # I am not sure if this is correct, but it makes sense to me.
            return min(p_val_one, p_val_two)
    def serial_check(self):
        """
        This is a test method for the serial test based on the examples in the NIST documentation
        :return:
        """
        expected = [0.143005, 0.766182, 0.861925, 0.157500]
        self.generic_checker("Check Serial Test", expected, self.serial)
    def approximate_entropy(self, bin_data: str, pattern_length=10):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        As with the Serial test of Section 2.11, the focus of this test is the
        frequency of all possible overlapping m-bit patterns across the entire sequence.
        The purpose of the test is to compare the frequency of overlapping
        blocks of two consecutive/adjacent lengths (m and m+1) against the
        expected result for a random sequence.
        :param bin_data: a binary string
        :param pattern_length: the length of the pattern (m)
        :return: the P value
        """
        bin_data_len = len(bin_data)
        # Add first m+1 bits to the end
        # NOTE: documentation says m-1 bits but that doesnt make sense, or work.
        bin_data += bin_data[:pattern_length + 1:]
        # Get max length one patterns for m, m-1, m-2
        max_pattern = ''
        for i in range(pattern_length + 2):
            max_pattern += '1'
        # Keep track of each pattern's frequency (how often it appears)
        vobs_one = numpy.zeros(int(max_pattern[0:pattern_length:], 2) + 1)
        vobs_two = numpy.zeros(int(max_pattern[0:pattern_length + 1:], 2) + 1)
        for i in range(bin_data_len):
            # Work out what pattern is observed
            vobs_one[int(bin_data[i:i + pattern_length:], 2)] += 1
            vobs_two[int(bin_data[i:i + pattern_length + 1:], 2)] += 1
        # Calculate the test statistics and p values
        vobs = [vobs_one, vobs_two]
        sums = numpy.zeros(2)
        for i in range(2):
            for j in range(len(vobs[i])):
                if vobs[i][j] > 0:
                    sums[i] += vobs[i][j] * math.log(vobs[i][j] / bin_data_len)
        sums /= bin_data_len
        ape = sums[0] - sums[1]
        chi_squared = 2.0 * bin_data_len * (math.log(2) - ape)
        p_val = spc.gammaincc(pow(2, pattern_length - 1), chi_squared / 2.0)
        return p_val
    def approximate_entropy_check(self):
        """
        This is a test method for the serial test based on the examples in the NIST documentation
        :return:
        """
        expected = [0.361595, 0.700073, 0.884740, 0.180481]
        self.generic_checker("Check Approximate Entropy Test", expected, self.approximate_entropy)
    def cumulative_sums(self, bin_data: str, method="forward"):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the maximal excursion (from zero) of the
        random walk defined by the cumulative sum of adjusted (-1, +1) digits
        in the sequence. The purpose of the test is to determine whether the
        cumulative sum of the partial sequences occurring in the tested sequence
        is too large or too small relative to the expected behavior of that
        cumulative sum for random sequences. This cumulative sum may be considered
        as a random walk. For a random sequence, the excursions of the random
        walk should be near zero. For certain types of non-random
        sequences, the excursions of this random walk from zero will be large.
        :param bin_data: a binary string
        :param method: the method used to calculate the statistic
        :return: the P-value
        """
        bin_data_len = len(bin_data)
        counts = numpy.zeros(bin_data_len)
        # Calculate the statistic using a walk forward
        if method != "forward":
            bin_data = bin_data[::-1]
        ix = 0
        for char in bin_data:
            sub = 1
            if char == '0':
                sub = -1
            if ix > 0:
                counts[ix] = counts[ix - 1] + sub
            else:
                counts[ix] = sub
            ix += 1
        # This is the maximum absolute level obtained by the sequence
        abs_max = numpy.max(numpy.abs(counts))
        start = int(numpy.floor(0.25 * numpy.floor(-bin_data_len / abs_max) + 1))
        end = int(numpy.floor(0.25 * numpy.floor(bin_data_len / abs_max) - 1))
        terms_one = []
        for k in range(start, end + 1):
            sub = sst.norm.cdf((4 * k - 1) * abs_max / numpy.sqrt(bin_data_len))
            terms_one.append(sst.norm.cdf((4 * k + 1) * abs_max / numpy.sqrt(bin_data_len)) - sub)
        start = int(numpy.floor(0.25 * numpy.floor(-bin_data_len / abs_max - 3)))
        end = int(numpy.floor(0.25 * numpy.floor(bin_data_len / abs_max) - 1))
        terms_two = []
        for k in range(start, end + 1):
            sub = sst.norm.cdf((4 * k + 1) * abs_max / numpy.sqrt(bin_data_len))
            terms_two.append(sst.norm.cdf((4 * k + 3) * abs_max / numpy.sqrt(bin_data_len)) - sub)
        p_val = 1.0 - numpy.sum(numpy.array(terms_one))
        p_val += numpy.sum(numpy.array(terms_two))
        return p_val
    def cumulative_sums_check(self):
        """
        This is a test method for the serial test based on the examples in the NIST documentation
        :return:
        """
        # Assumes forward method used
        expected = [0.628308, 0.669887, 0.879009, 0.917121]
        self.generic_checker("Check Cumulative Sums Test", expected, self.cumulative_sums)
        # For backward method uncomment these and change default method
        # expected = [0.663369, 0.724266, 0.957206, 0.689519]
        # self.generic_checker("Check Cumulative Sums Test", expected, self.cumulative_sums)
    def random_excursions(self, bin_data):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the number of cycles having exactly K visits
        in a cumulative sum random walk. The cumulative sum random walk is
        derived from partial sums after the (0,1) sequence is transferred to the
        appropriate (-1, +1) sequence. A cycle of a random walk consists of a
        sequence of steps of unit length taken at random that begin at and return
        to the origin. The purpose of this test is to determine if the number of visits
        to a particular state within a cycle deviates from what one would expect for a
        random sequence. This test is actually a series of eight tests (and conclusions),
        one test and conclusion for each of the states:
        States -> -4, -3, -2, -1 and +1, +2, +3, +4.
        :param bin_data: a binary string
        :return: the P-value
        """
        # Turn all the binary digits into +1 or -1
        int_data = numpy.zeros(len(bin_data))
        for i in range(len(bin_data)):
            if bin_data[i] == '0':
                int_data[i] = -1.0
            else:
                int_data[i] = 1.0
        # Calculate the cumulative sum
        cumulative_sum = numpy.cumsum(int_data)
        # Append a 0 to the end and beginning of the sum
        cumulative_sum = numpy.append(cumulative_sum, [0])
        cumulative_sum = numpy.append([0], cumulative_sum)
        # These are the states we are going to look at
        x_values = numpy.array([-4, -3, -2, -1, 1, 2, 3, 4])
        # Identify all the locations where the cumulative sum revisits 0
        position = numpy.where(cumulative_sum == 0)[0]
        # For this identify all the cycles
        cycles = []
        for pos in range(len(position) - 1):
            # Add this cycle to the list of cycles
            cycles.append(cumulative_sum[position[pos]:position[pos + 1] + 1])
        num_cycles = len(cycles)
        state_count = []
        for cycle in cycles:
            # Determine the number of times each cycle visits each state
            state_count.append(([len(numpy.where(cycle == state)[0]) for state in x_values]))
        state_count = numpy.transpose(numpy.clip(state_count, 0, 5))
        su_array = []
        for cycle in range(6):
            su_array.append([(sct == cycle).sum() for sct in state_count])
        su_array = numpy.transpose(su_array)
        piks = ([([self.get_pik_value(uu, state) for uu in range(6)]) for state in x_values])
        inner_term = num_cycles * numpy.array(piks)
        chi = numpy.sum(1.0 * (numpy.array(su_array) - inner_term) ** 2 / inner_term, axis=1)
        p_values = ([spc.gammaincc(2.5, cs / 2.0) for cs in chi])
        return p_values
    def get_pik_value(self, k, x):
        """
        This method is used by the random_excursions method to get expected probabilities
        """
        if k == 0:
            out = 1 - 1.0 / (2 * numpy.abs(x))
        elif k >= 5:
            out = (1.0 / (2 * numpy.abs(x))) * (1 - 1.0 / (2 * numpy.abs(x))) ** 4
        else:
            out = (1.0 / (4 * x * x)) * (1 - 1.0 / (2 * numpy.abs(x))) ** (k - 1)
        return out
    def random_excursions_check(self):
        """
        This method is used to check the random_excursions method is working as expected
        :return: None
        """
        expected = [0.844143, 0.786868, 0.216235, 0.783283]
        p_values = []
        data_sets = ["pi", "e", "sqrt2", "sqrt3"]
        for data_set in data_sets:
            data = self.load_test_data(data_set)[:1000000]
            p_values.append(self.random_excursions(data)[4])
        self.generic_checker("Random Excursions Test", expected, self.random_excursions, p_values)
    def random_excursions_variant(self, bin_data):
        """
        Note that this description is taken from the NIST documentation
        http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
        The focus of this test is the total number of times that a particular
        state is visited (i.e., occurs) in a cumulative sum random walk.
        The purpose of this test is to detect deviations from the expected number of
        visits to various states in the random walk.
        This test is actually a series of eighteen tests (and conclusions), one
        test and conclusion for each of the states: -9, -8, …, -1 and +1, +2, …, +9.
        :param bin_data: a binary string
        :return: the P-value
        """
        int_data = numpy.zeros(len(bin_data))
        for i in range(len(bin_data)):
            int_data[i] = int(bin_data[i])
        sum_int = (2 * int_data) - numpy.ones(len(int_data))
        cumulative_sum = numpy.cumsum(sum_int)
        li_data = []
        for x_sum in sorted(set(cumulative_sum)):
            if numpy.abs(x_sum) <= 9:
                li_data.append([x_sum, len(numpy.where(cumulative_sum == x_sum)[0])])
        j = self.get_frequency(li_data, 0) + 1
        p_values = []
        for x_sum in range(-9, 9 + 1):
            if not x_sum == 0:
                den = numpy.sqrt(2 * j * (4 * numpy.abs(x_sum) - 2))
                p_values.append(spc.erfc(numpy.abs(self.get_frequency(li_data, x_sum) - j) / den))
        return p_values
    def get_frequency(self, list_data, trigger):
        """
        This method is used by the random_excursions_variant method to get frequencies
        """
        frequency = 0
        for (x_trigger, y_frequency) in list_data:
            if x_trigger == trigger:
                frequency = y_frequency
        return frequency
    def random_excursions_variant_check(self):
        """
        This method is used to check the random_excursions_variant method is working as expected
        :return: None
        """
        expected = [0.760966, 0.826009, 0.566118, 0.155066]
        p_values = []
        data_sets = ["pi", "e", "sqrt2", "sqrt3"]
        for data_set in data_sets:
            data = self.load_test_data(data_set)[:1000000]
            p_values.append(self.random_excursions_variant(data)[8])
        self.generic_checker("Random Excursions Variant Test", expected,
            self.random_excursions, p_values)
class BinaryMatrix:
    """
    This class contains the algorithm specified in the NIST suite for computing
    the **binary rank** of a matrix.
    """
    def __init__(self, matrix, rows, cols):
        """
        Initialize the matrix.
        :param matrix: the matrix we want to compute the rank for
        :param rows: the number of rows
        :param cols: the number of columns
        :return: a BinaryMatrix object
        """
        self.num_rows = rows
        self.num_cols = cols
        self.matrix = matrix
        self.min_rank = min(rows, cols)
    def compute_rank(self, verbose=False):
        """
        This method computes the binary rank of self.matrix
        :param verbose: if this is true it prints out the matrix after the forward
        elimination and backward elimination operations on the rows.
        This was used to testing the method to check it is working as expected.
        :return: the rank of the matrix.
        """
        if verbose:
            print("Original Matrix\n", self.matrix)
        i = 0
        while i < self.min_rank - 1:
            if self.matrix[i][i] == 1:
                self.perform_row_operations(i, True)
            else:
                found = self.find_unit_element_swap(i, True)
                if found == 1:
                    self.perform_row_operations(i, True)
            i += 1
        if verbose:
            print("Intermediate Matrix\n", self.matrix)
        i = self.min_rank - 1
        while i > 0:
            if self.matrix[i][i] == 1:
                self.perform_row_operations(i, False)
            else:
                if self.find_unit_element_swap(i, False) == 1:
                    self.perform_row_operations(i, False)
            i -= 1
        if verbose:
            print("Final Matrix\n", self.matrix)
        return self.determine_rank()
    def perform_row_operations(self, i, forward_elimination):
        """
        This method performs the elementary row operations.
        This involves xor'ing up to two rows together depending on
        whether or not certain elements in the matrix contain 1's
        if the"current" element does not.
        :param i: the current index we are are looking at
        :param forward_elimination: True or False.
        """
        if forward_elimination:
            j = i + 1
            while j < self.num_rows:
                if self.matrix[j][i] == 1:
                    self.matrix[j, :] = (self.matrix[j, :] + self.matrix[i, :]) % 2
                j += 1
        else:
            j = i - 1
            while j >= 0:
                if self.matrix[j][i] == 1:
                    self.matrix[j, :] = (self.matrix[j, :] + self.matrix[i, :]) % 2
                j -= 1
    def find_unit_element_swap(self, i, forward_elimination):
        """
        This given an index which does not contain a 1 this searches through the rows
        below the index to see which rows contain 1's, if they do then they swapped.
        This is done on the forward and backward elimination
        :param i: the current index we are looking at
        :param forward_elimination: True or False.
        """
        row_op = 0
        if forward_elimination:
            index = i + 1
            while index < self.num_rows and self.matrix[index][i] == 0:
                index += 1
            if index < self.num_rows:
                row_op = self.swap_rows(i, index)
        else:
            index = i - 1
            while index >= 0 and self.matrix[index][i] == 0:
                index -= 1
            if index >= 0:
                row_op = self.swap_rows(i, index)
        return row_op
    def swap_rows(self, row_one, row_two):
        """
        This method just swaps two rows in a matrix.
        Had to use the copy package to ensure no memory leakage
        :param row_one: the first row we want to swap and
        :param row_two: the row we want to swap it with
        :return: 1
        """
        temp = copy.copy(self.matrix[row_one, :])
        self.matrix[row_one, :] = self.matrix[row_two, :]
        self.matrix[row_two, :] = temp
        return 1
    def determine_rank(self):
        """
        This method determines the rank of the transformed matrix
        :return: the rank of the transformed matrix
        """
        rank = self.min_rank
        i = 0
        while i < self.num_rows:
            all_zeros = 1
            for j in range(self.num_cols):
                if self.matrix[i][j] == 1:
                    all_zeros = 0
            if all_zeros == 1:
                rank -= 1
            i += 1
        return rank
def test_binary_matrix():
    """
    This is just a silly method for testing the matrix rank class. It is redundant since
    the Binary Matrix Rank test passes the unit tests on the test data anyway ... still
    useful to keep around though.
    :return:
    """
    data = [1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 1,
            1, 0, 1, 0, 1, 0,
            0, 0, 1, 0, 1, 1,
            0, 0, 0, 0, 1, 0]
    data_matrix = numpy.array(data)
    data_matrix = data_matrix.reshape((6, 6))
    ranker = BinaryMatrix(m, 6, 6)
    print(ranker.compute_rank(verbose=True))
#
# This is where the packet inspection code begins.
# This was written by Andrew Pitonyak for Battelle.
#
#Traverses through all layers in a single packet (input) and returns one layer at a time.
def get_packet_layers(read_packet, packet_number):
    """Get all packet layers"""
    while True:
        layer = read_packet.getlayer(packet_number)
        if layer is None:
            break
        yield layer
        packet_number += 1
def get_packet_layers_name(read_packet, layer_names):
    """Get the layer names from a packet"""
    for layer in get_packet_layers(read_packet, 0):
        if (layer.name in layer_names.keys()):
            layer_names[layer.name] = layer_names[layer.name] + 1
        else:
            layer_names[layer.name] = 1
    return layer_names
def use_this_ip(ip_string, ip_list):
    """
    Check to see if this ip is included in the ip list.
    If the length of the string is zero, return True.
    If the nothing is in the ip list, then return True.
    If the ip string starts with any of the values listed
    in the ip list, return True.
    """
    if len(ip_string) == 0 or len(ip_list) == 0:
        return True
    for ip_address in ip_list:
        if ip_string.startswith(ip_address):
            return True
    return False
def main():
    """
    Mail program to parse command line input.
    """
    rng_tester = RandomnessTester(None)

    # The code below will run a test against the random number generator
    #percent_passed, csv_string = rng_tester.run_test_against_random_bits(True, 128)
    #print("Percent passed: " + str(percent_passed))
    #print(csv_string)
    #print(rng_tester.get_csv_header())

    parser = ArgumentParser()
    parser.add_argument('-f', '--file', help='Path to input PCAP file', required=True)
    parser.add_argument('-o', '--output', help='File name for output CSV file', default= "")
    parser.add_argument('-s', '--source', help=
        'comma delimted list of valid source IP addresses, source must be one of these',
        default= "")
    parser.add_argument('-d', '--destination', help=
        'comma delimted list of valid destination IP addresses, destination must be one of these',
        default= "")
    args = parser.parse_args()
    # TODO: parse the source and destination IP addresses and
    # then ignore all packets that do not match.
    output_file = args.output if len(args.output) > 0 else args.file + ".csv"
    if not exists(args.file):
        print("\n\nInput pcap file does not exist: " + args.file + "\n\n")
        parser.print_help(sys.stderr)
        sys.exit(1)
    print("\n\nReading from " + args.file)
    print("Writing to " + output_file)
    src_ips = args.source.split(',')
    dst_ips = args.destination.split(',')
    counter = 0
    encrypted_counter = 0
    start_time = time.time()
    # Ignore all packets that do not have both an IP layer and an ESP layer.
    with open(output_file, 'w') as csv_file:
        csv_file.write(rng_tester.get_csv_header() + "\n")
        for read_packet in PcapNgReader(args.file):
            #filters to ensure we are examining IP and
            #Encrypted read_packets (protocol 50) packets
            counter = counter + 1
            if not read_packet.haslayer("IP"):
                continue
            ip_layer = read_packet.getlayer("IP")
            # same as using:
            # if ip_layer.proto != 50:
            if not read_packet.haslayer("ESP"):
                continue
            #if ip_layer.proto != 50:
            #    continue
            print("Encrypted packet at index " + str(counter - 1) + " " +\
                  ip_layer.src + " ==> " + ip_layer.dst)
            #
            # Check source IPs
            #
            if not use_this_ip(ip_layer.src, src_ips) or not use_this_ip(ip_layer.dst, dst_ips):
                continue
            #
            # This has an ESP packet.
            # It MIGHT have a Padding layer.
            #
            encrypted_counter = encrypted_counter + 1
            #for layer in get_packet_layers(read_packet, 0):
            #	print("     Layer: " + layer.name)
            esp_layer = read_packet.getlayer("ESP")
            #print("esp_layer.spi: " + str(esp_layer.spi))
            #print("for esp_layer.seq: " + str(esp_layer.seq))
            # This is class bytes:
            esp_data = esp_layer.data
            print("from esp_layer pay load len = " + str(len(esp_data)))
            print(esp_data)
            #
            # esp_data is of type (class) bytes, which is exactly what I need.
            # Change False to True to print the full export data.
            #
            percent_passed, csv_string = rng_tester.test_byte_array(False, esp_data)
            #
            # Convert the data to a string, then grab the first 256 parts of the string.
            # Deal with it later.
            #
            short_data = esp_data if len(esp_data) < 257 else esp_data[:256]
            csv_file.write(str(counter - 1) + ', ' + csv_string + ', "' + \
                           rng_tester.csv_safe_string(args.file) + '", "' + ip_layer.src + \
                           '", "' + ip_layer.dst + '",' + str(len(esp_data)) + ',"' + \
                           rng_tester.csv_safe_string(str(short_data)) + '"\n')
            if read_packet.haslayer("Padding"):
                padding_layer = read_packet.getlayer("Padding")
                # Type is class bytes
                padding = padding_layer.load
                print("Padding length: " + str(len(padding)) + " ==> " + str(padding))
    end_time = time.time() - start_time
    print("Total Read time: " + str(end_time) + " for " + str(counter) + \
          " encrypted:" + str(encrypted_counter))
if __name__ == '__main__':
    main()
