# Green Team Toolbox

This is a project for tools that may help Green Teams.  Tools may be either collected from the Internet or internally developed.

The base files (by name) are as follows:

| file / directory  | Description  |
|-------------------|--------------|
| ciscot7.py        | Decrypt insecure (password 7) Cisco passwords. |
| encrypted.py      | Test randomness primarily aimed at the ESP layer using IP protocol number 50. |
| Find_MAC_IP       | Original program to find MAC and IP files. Deprecated, use the Heuristics project|
| Heuristics        | Finds known exploits by examinging PCAP files. |
| kmeans.py         | Generate an annotated GraphML file from an input JSON file. |
| README.md         | This file. |
| test_encrypted.py | Unit tests for encrypted.py |

The individual tools are listed below.

## Randomness Testing IPSec With encrypted.py

Test randomness primarily aimed at the Encapsulating Security Protocol (ESP) layer using IP number 50. Data in an ESP packet is encrypted and should, therefore, appear random. Mathematically speaking, a statistic test descxribes how closely the distribution of the data (in our case, encrypted data from the ESP layer), matches the distribution predicted under the null hypothesis of the statistical test used.

This program reads a PCAP file and tests the encrypted data of the ESP layer for randomness. Packets without an ESP layer are ignored.Although the primary purpose of encrypted.py is to test a PCAP files for appropriate randomness (in case data has been modified), it can be used to test any data as is demonstrated in test_encrypted.py.

Each of the tests reduces the data to a bit stream of zeros and ones and tests the bit stream for randomness. Even the matrix rank tests transform the data into a series of 32 x 32 matrices of ones and zeros.

The National Institute of Standards and Technology (NIST) published a methodology to test data for randomness The document [nistspecialpublication800-22r1a.pdf](https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-22r1a.pdf) is included in this directory of the repository. The intended purpose of the paper is to test random number generators to see if they are random. Fifteen separate tests are presented to test a random sequence.

The original code was pulled from git with a write-up by the author [hacking the random walk hypothesis](http://www.turingfinance.com/hacking-the-random-walk-hypothesis/). Pull the code as follows:

    git clone https://github.com/StuartGordonReid/r4nd0m.git

A CSV file is generated with 51 columns.

| Col| Title | Description |
|----|-------|-------------|
| 0  | Index | Zero based index to the packet in the pcap file.|
| 1  | Percent | Percent Passed |
| 2  | Num Passed | Number of tests that passed |
| 3  | Num Skipped | Number of tests skipped because they were considered not appropriate for the test. |
| 4  | Num Failed | Number of tests that failed the randomness test. |
| 5  | Confidence Level| Confidence level used for the tests, probably 0.005. |
| 6  | Monobit Test | Determine if the number of ones and zeros in a sequence are approximately the same as would be expected. All tests depend on this test. The P-value should be ≥ 0.01 to accept the value as random.  |
| 7  | Block Frequency Test | Determine if the number of ones and zeros in each of M non-overlapping blocks created from a sequence appear to have a random distribution.|
| 8  | Independent Runs Test | A run is a series of increasing (or decreasing) values. This test looks at the number and length of runs; see [Runs Test for Detecting Non-randomness](https://www.itl.nist.gov/div898/handbook/eda/section3/eda35d.htm) |
| 9  | Longest Runs Test | See Indep4endent Runts test above. |
| 10 | Matrix Rank Test | Check for a linear dependency among the fixed length sub-strings of the data. There is an expected distribution of rank for a random bit stream. |
| 11 | Spectral Test | Use a Discrete Fourier Transform to look at the frequency components of the data bit stream. |
| 12 | Non Overlapping Patterns Test | A sliding window looks for too many occurrences of non-periodic patterns. Returns a measure of how well the number of hits matches the expected. |
| 13 | Overlapping Patterns Test | Similar to the Non Overlapping Patterns Test, but looks at the number of non-occurrences of patterns. |
| 14 | Universal Test | Detect if the sequence can be significantly compressed; random data should not be easily compressible. |
| 15 | Linear Complexity Test | Use an LFSR to Determine if the sequence is complex enough to be considered random. |
| 16 | Serial Test (p01) | Look at the frequencey of all possible overlapping m-bit patterns across the sequence. This looks at p-value 1. |
| 17 | Serial Test (p02) | Look at the frequencey of all possible overlapping m-bit patterns across the sequence. This looks at p-value 2. |
| 18 | Approximate Entropy Test | Similar to the Serial Test, but compare the frequency of overlapping blocks of two consecutive / adjacent lengths against the expected result for a random sequence. |
| 19 | Cumulative Sums Test (Forward) | The maximal excursion (from zero) of the random walk moving forward. Result should be near zero. |
| 20 | Cumulative Sums Test (Backward) | The maximal excursion (from zero) of the random walk moving backwards. Result should be near zero. |
| 21 | Random Excursions Test (p01) | See Random Excursions Test (p01) |
| 22 | Random Excursions Test (p02) | See Random Excursions Test (p01) |
| 23 | Random Excursions Test (p03) | See Random Excursions Test (p01) |
| 24 | Random Excursions Test (p04) | See Random Excursions Test (p01) |
| 25 | Random Excursions Test (p05) | See Random Excursions Test (p01) |
| 26 | Random Excursions Test (p06) | See Random Excursions Test (p01) |
| 27 | Random Excursions Test (p07) | See Random Excursions Test (p01) |
| 28 | Random Excursions Test (p08) | See Random Excursions Test (p01) |
| 29 | Random Excursions Variant Test (p01) | Test total number of times a state is visited in a cumulative sum random walk. Looks at states -9 to +9. |
| 30 | Random Excursions Variant Test (p02) | See Random Excursions Variant Test (p01) |
| 31 | Random Excursions Variant Test (p03) | See Random Excursions Variant Test (p01) |
| 32 | Random Excursions Variant Test (p04) | See Random Excursions Variant Test (p01) |
| 33 | Random Excursions Variant Test (p05) | See Random Excursions Variant Test (p01) |
| 34 | Random Excursions Variant Test (p06) | See Random Excursions Variant Test (p01) |
| 35 | Random Excursions Variant Test (p07) | See Random Excursions Variant Test (p01) |
| 36 | Random Excursions Variant Test (p08) | See Random Excursions Variant Test (p01) |
| 37 | Random Excursions Variant Test (p09) | See Random Excursions Variant Test (p01) |
| 38 | Random Excursions Variant Test (p10) | See Random Excursions Variant Test (p01) |
| 39 | Random Excursions Variant Test (p11) | See Random Excursions Variant Test (p01) |
| 40 | Random Excursions Variant Test (p12) | See Random Excursions Variant Test (p01) |
| 41 | Random Excursions Variant Test (p13) | See Random Excursions Variant Test (p01) |
| 42 | Random Excursions Variant Test (p14) | See Random Excursions Variant Test (p01) |
| 43 | Random Excursions Variant Test (p15) | See Random Excursions Variant Test (p01) |
| 44 | Random Excursions Variant Test (p16) | See Random Excursions Variant Test (p01) |
| 45 | Random Excursions Variant Test (p17) | See Random Excursions Variant Test (p01) |
| 46 | File | PCAP file name |
| 47 | Source IP | Source IP addresses. |
| 48 | Dest IP | Destination IP address. |
| 49 | Data Len | Total length of the ESP data. |
| 50 | Data  |First few bytes of the data. |

A test with a -1 for the value means that the test was skipped. A p-value ≥ 0.01 is a passing value. If the mono-bit test (number of zeros and ones) fails, then the data is probably not random. 

### Using the program

This program uses external Python packages such as scapy, numpy, and scipy. The following arguments are supported:

| parameter | Description |
|-----------|-------------|
| -f | Path to the input PCAP file. This is required.|
| -o | Filename for the output CSV file.|
| -s | Comma delimited list of valid source IP addresses. If an IP address is included, packets that do | not have an approved source IP addresses are skipped / ignored. |
| -d | Comma delimited list of valid destination IP addresses. If an IP address is included, packets that do not have an approved destination IP addresses are skipped / ignored. |




## Tools by Category


### Internet Tools
- `ciscot7.py` <br>
This is a tool designed to decrypt insecure (password 7) Cisco passwords.

### Internally developed
- Topology Reconstruction
- Information Graph
- Configuration File Parsing
- Randomness Assessment for ESP Payload data

# Acronyms

| Acronym | Description |
|---------|-------------|
| ESP     | IP Encapsulating Security Protocol is designed to provide a mix of security services (encryption) in IPv4 and IPv6. |
| IP      | The Internet Protocol (IP) is the network layer communications protocol in the Internet protocol suite for relaying datagrams across network boundaries. Its routing function enables internetworking, and essentially establishes the Internet. |
| LFSR    | A Linear Feedbak Shift Register is a shift register whose input bit is a linear function of its previous state. |
| NIST    | National Institute of Standards and Technology. |
| PCAP  | Packet Capture files are readable and writable by by wireshark. |


