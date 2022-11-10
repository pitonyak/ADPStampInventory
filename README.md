# Green Team Toolbox

This is a project for tools that may help Green Teams.  Tools may be either collected from the Internet or internally developed.

The base files (by name) are as follows: <br/>

| file / directory  | Description  |
|-------------------|--------------|
| ciscot7.py        | Decrypt insecure (password 7) Cisco passwords. |
| encrypted.py      | Test randomness primarily aimed at the ESP layer using IP protocol number 50. There is a sample output file sample_encrypted.csv and a test program test_encrypted.py. |
| Find_MAC_IP       | Original program to find MAC and IP files. Deprecated, use the Heuristics project. |
| Heuristics        | Finds known exploits by examinging PCAP files. |
| isakmp            | Identifies devices of interest based on ISAKMP vendor ID. |
| kmeans.py         | Generate an annotated GraphML file from an input JSON file. |
| README.md         | This file. |
| string_probability.py | Determine the probability that a strings of a given length will be seen a specific number of times in data of a specified length. |
| test_encrypted.py | Test encrypted.py |
| nistspecialpublication800-22r1a.pdf | NIST document related to encrypted.py indicating how to test for randomness. |

The individual tools are listed below.

## Compiler Versions and Environment

The target evironment is Ubuntu 20.0.4, but the client is probably running stock RHEL that has been stigged. Ubuntu 20.0.4 has:

- On Linux, to check your OS and release, use `cat /etc/os-release` or even `cat /etc/os-release | grep PRETTY_NAME` to only see the name.
- gcc 9.4.0, which supports c++17 when using the `-std=c++17` as a compile time flag. Use `gcc --version` to see the installed version.
- Python 3. Ubuntu 20.0.4 shows Python 3.8.10. Use `python3 --version` to see the installed version.
- Optimizations are turned on for C++ using (`-O3`) because speed is important while processing large PCAP files.
- While compiling, enable warning messages (`-Werror -Wall -Wextra`) to help find errors.
- In test and debug, allow for memory error checking (`-fsanitize=undefined,address -fno-sanitize=alignment`) but this seriously affects runtime so do not include this in production or while dealing with large files. Enabling sanitize will probably require installing another package or there will be link errors.
- Make is currently used to build but there is a move to use cmake, so install cmake.

To summarize, on Ubuntu 20.0.4, install a standard C++ environment with gcc, g++, Python 3, make, and cmake.

### C++ Libraries

On Ubuntu, install the libraries: 

- libpcap-dev

Other libraries are expected to be used for some in-progress projects, and this list will be updated.

### Python Modules

Usually pip is used to install packages for Python; on Ubuntu, use "sudo apt install python3-pip". 
The following Python modules should be installed on a standard system for Python using pip install:

- contourpy
- cycler
- fonttools
- kiwisolver
- matplotlib
- networkx
- numpy  (On some systems, may need to use *python3 -m pip install numpy -I* if there is an error.)
- packaging
- pyparsing
- scapy
- scikit-learn
- scipy
- sklearn

Although it is not used by the Python I tried, notes state that the 
**tensorflow** package is used for some package.

### Environment Testing

A good way to test for Python is to simply run the Python code. 
Even without using parameters, errors are printed if a required library is not present. 
This document lists how to use each of the programs, but the intent here is for something 
simple that checks for the correct libraries. 
Look elsewhere for details and use this list to test for properly installed libraries. 

| test      | Comment     |
|-----------|-------------|
| python3 test_encrypted.py | Ignore comments about deprecated methods |
| python3 encrypted.py -h  |  Already tested with test_encrypted, but a more complete test would be: *python3 encrypted.py -f in.pcap -o out.csv* |
| python3 string_probability.py -h  | For a more throrough test, use *python3 string_probability.py -s 4 -n 4 10 -d 30*, which may show a few unimportant errors. |
| python3 kmeans.py -h  | Print help and check imports. |
| python3 ciscot7.py --help | Print help and check imports.  |

To test build ability for C++, start with the Heuristics directory. 

| test      | Comment     |
|-----------|-------------|
| cp Makefile.dat Makefile | Get the Makefile ready |
| make | Perform a build, which tests that libpcap is available for linking |
| ./utilities_test | Check file and directory permissions may fail on Ubuntu because of unexpected permissions to things that were not expected; for example, ability to write to a few specific directories. |
| ./crc_test crc_test | Calculate the crc of the file crc_test. To verify, install the crc32 command and use that to test for correct 32-bit CRC. |
| ./heuristics -r in.pcap -a out.pcap | This will read the input pcap file, generate the IP and MAC files in the same directory as the PCAP file and also generate an output PCAP file named out.pcap. |

 

## Generating a GraphML file using kmeans.py

kmeans.py uses sklearn. Although the sklearn package is the same as scikit-learn package and you are able to install either, it is better to install scikit-learn. If you install sklearn, it will install scikit-learn and when you use `pip list`, it will show sklearn with version 0.0 and scikit-learn with whatever version you installed. Use `pip install scikit-learn` to install the required sklearn dependency.


kmeans.py supports the following arguments: <br/>

| parameter | Description |
|-----------|-------------|
| -h        | Print usage. |
| -f        | Path to input JSON file.. |
| -d        | Directory of the input JSON files. |
| -o        | Filename for the Output file. |
| -c        | Number of kmeans clusters to use. |

TODO: Add a sample input file for kmeans.py.

## Randomness Testing IPSec With encrypted.py

Test randomness primarily aimed at the Encapsulating Security Protocol (ESP) layer using IP number 50. Data in an ESP packet is encrypted and should, therefore, appear random. Mathematically speaking, a statistic test descxribes how closely the distribution of the data (in our case, encrypted data from the ESP layer), matches the distribution predicted under the null hypothesis of the statistical test used.

This program reads a PCAP file and tests the encrypted data of the ESP layer for randomness. Packets without an ESP layer are ignored.Although the primary purpose of encrypted.py is to test a PCAP files for appropriate randomness (in case data has been modified), it can be used to test any data as is demonstrated in test_encrypted.py.

Each of the tests reduces the data to a bit stream of zeros and ones and tests the bit stream for randomness. Even the matrix rank tests transform the data into a series of 32 x 32 matrices of ones and zeros.

The National Institute of Standards and Technology (NIST) published a methodology to test data for randomness The document [nistspecialpublication800-22r1a.pdf](https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-22r1a.pdf) is included in this directory of the repository. The intended purpose of the paper is to test random number generators to see if they are random. Fifteen separate tests are presented to test a random sequence.

The original code was pulled from git with a write-up by the author [hacking the random walk hypothesis](http://www.turingfinance.com/hacking-the-random-walk-hypothesis/). Pull the code as follows:

    git clone https://github.com/StuartGordonReid/r4nd0m.git

A CSV file is generated with the columns shown in the table below. If the CSV is loaded into a spreadsheet such as Excel or Calc, the first column below (Col) identifies the spreadsheet column numbers (A, B, ... AZ). The second column (#) identifies the numeric index (starting at zero) of the column. The third column (Title) is the column title except in the spread sheet. Note that each individual test result has a number in front of it. The number identifies a test as enumerated in the NIST document. Some tests return variations or have multiple returns values and these are listed as the same test number; for example, number 13 has results for running the Cumulative Sums Test forwards and backwards. The fourth column has a brief description of what the column contains; refer to the NIST document for a more complete description. <br/>

| Col | #  | Title      | Description |
|-----|----|------------|-------------|
|  A  | 0  | Index      | Zero based index to the packet in the pcap file.|
|  B  | 1  | Percent    | Percent Passed |
|  C  | 2  | Num Passed | Number of tests that passed |
|  D  | 3  | Num Skipped | Number of tests skipped because they were considered not appropriate for the test. |
|  E  | 4  | Num Failed | Number of tests that failed the randomness test. |
|  F  | 5  | Confidence Level| Confidence level used for the tests, probably 0.005. |
|  G  | 6  | 01. Monobit Test | Determine if the number of ones and zeros in a sequence are approximately the same as would be expected. All tests depend on this test. The P-value should be ≥ 0.01 to accept the value as random.  |
|  H  | 7  | 02. Block Frequency Test | Determine if the number of ones and zeros in each of M non-overlapping blocks created from a sequence appear to have a random distribution.|
|  I  | 8  | 03. Independent Runs Test | A run is a series of increasing (or decreasing) values. This test looks at the number and length of runs; see [Runs Test for Detecting Non-randomness](https://www.itl.nist.gov/div898/handbook/eda/section3/eda35d.htm) |
|  J  | 9  | 04. Longest Runs Test | See Indep4endent Runts test above. |
|  K  | 10 | 05. Matrix Rank Test | Check for a linear dependency among the fixed length sub-strings of the data. There is an expected distribution of rank for a random bit stream. |
|  L  | 11 | 06. Spectral Test | Use a Discrete Fourier Transform to look at the frequency components of the data bit stream. |
|  M  | 12 | 07. Non Overlapping Patterns Test | A sliding window looks for too many occurrences of non-periodic patterns. Returns a measure of how well the number of hits matches the expected. |
|  N  | 13 | 08. Overlapping Patterns Test | Similar to the Non Overlapping Patterns Test, but looks at the number of non-occurrences of patterns. |
|  O  | 14 | 09. Universal Test | Detect if the sequence can be significantly compressed; random data should not be easily compressible. |
|  P  | 15 | 10. Linear Complexity Test | Use an LFSR to Determine if the sequence is complex enough to be considered random. |
|  Q  | 16 | 11. Serial Test (p01) | Look at the frequencey of all possible overlapping m-bit patterns across the sequence. This looks at p-value 1. |
|  R  | 17 | 11. Serial Test (p02) | Look at the frequencey of all possible overlapping m-bit patterns across the sequence. This looks at p-value 2. |
|  S  | 18 | 12. Approximate Entropy Test | Similar to the Serial Test, but compare the frequency of overlapping blocks of two consecutive / adjacent lengths against the expected result for a random sequence. |
|  T  | 19 | 13. Cumulative Sums Test (Forward) | The maximal excursion (from zero) of the random walk moving forward. Result should be near zero. |
|  U  | 20 | 13. Cumulative Sums Test (Backward) | The maximal excursion (from zero) of the random walk moving backwards. Result should be near zero. |
|  V  | 21 | 14. Random Excursions Test (p01) | See Random Excursions Test (p01) |
|  W  | 22 | 14. Random Excursions Test (p02) | See Random Excursions Test (p01) |
|  X  | 23 | 14. Random Excursions Test (p03) | See Random Excursions Test (p01) |
|  Y  | 24 | 14. Random Excursions Test (p04) | See Random Excursions Test (p01) |
|  Z  | 25 | 14. Random Excursions Test (p05) | See Random Excursions Test (p01) |
| AA  | 26 | 14. Random Excursions Test (p06) | See Random Excursions Test (p01) |
| AB  | 27 | 14. Random Excursions Test (p07) | See Random Excursions Test (p01) |
| AC  | 28 | 14. Random Excursions Test (p08) | See Random Excursions Test (p01) |
| AD  | 29 | 15. Random Excursions Variant Test (p01) | Test total number of times a state is visited in a cumulative sum random walk. Looks at states -9 to +9. |
| AE  | 30 | 15. Random Excursions Variant Test (p02) | See Random Excursions Variant Test (p01) |
| AF  | 31 | 15. Random Excursions Variant Test (p03) | See Random Excursions Variant Test (p01) |
| AG  | 32 | 15. Random Excursions Variant Test (p04) | See Random Excursions Variant Test (p01) |
| AH  | 33 | 15. Random Excursions Variant Test (p05) | See Random Excursions Variant Test (p01) |
| AI  | 34 | 15. Random Excursions Variant Test (p06) | See Random Excursions Variant Test (p01) |
| AJ  | 35 | 15. Random Excursions Variant Test (p07) | See Random Excursions Variant Test (p01) |
| AK  | 36 | 15. Random Excursions Variant Test (p08) | See Random Excursions Variant Test (p01) |
| AL  | 37 | 15. Random Excursions Variant Test (p09) | See Random Excursions Variant Test (p01) |
| AM  | 38 | 15. Random Excursions Variant Test (p10) | See Random Excursions Variant Test (p01) |
| AN  | 39 | 15. Random Excursions Variant Test (p11) | See Random Excursions Variant Test (p01) |
| AO  | 40 | 15. Random Excursions Variant Test (p12) | See Random Excursions Variant Test (p01) |
| AP  | 41 | 15. Random Excursions Variant Test (p13) | See Random Excursions Variant Test (p01) |
| AQ  | 42 | 15. Random Excursions Variant Test (p14) | See Random Excursions Variant Test (p01) |
| AR  | 43 | 15. Random Excursions Variant Test (p15) | See Random Excursions Variant Test (p01) |
| AS  | 44 | 15. Random Excursions Variant Test (p16) | See Random Excursions Variant Test (p01) |
| AT  | 45 | 15. Random Excursions Variant Test (p17) | See Random Excursions Variant Test (p01) |
| AU  | 46 | 15. Random Excursions Variant Test (p18) | See Random Excursions Variant Test (p01) |
| AV  | 47 | File      | PCAP file name |
| AW  | 48 | Source IP | Source IP addresses. |
| AX  | 49 | Dest IP   | Destination IP address. |
| AY  | 50 | Data Len  | Total length of the ESP data. |
| AZ  | 51 | Data      | First few bytes of the data. |

A few notes to consider:

- The confidence Level is a value set in the program `self.confidence_level = 0.005` and this is used to determine if a test passes or fails, this is not a confidence level that a particular sample is random.
- A p-value of -1 means that the test was skipped. The program determined that the data was not suitable for the test; for example, perhaps the size is too small.
- A p-value ≥ 0.01 is a passing value for a specific test.
- The first test, Monobit or Frequency, is one of the most important tests. To quote from the NIST paper, *All subsequent tests are conditioned on having passed this first basic test.* In other words, if this test fails, the data is probably not random.

## Using test_encrypted.py

Run test_encrypted.py to verify that things are installed and working

~~~~
$ python3 test_encrypted.py
......
----------------------------------------------------------------------
Ran 6 tests in 0.001s

OK
~~~~

### Using encrypted.py

This program uses external Python packages such as scapy, numpy, and scipy. The following arguments are supported: <br/>

| parameter | Description |
|-----------|-------------|
| -f | Path to the input PCAP file. This is required.|
| -o | Filename for the output CSV file.|
| -s | Comma delimited list of valid source IP addresses. If an IP address is included, packets that do | not have an approved source IP addresses are skipped / ignored. |
| -d | Comma delimited list of valid destination IP addresses. If an IP address is included, packets that do not have an approved destination IP addresses are skipped / ignored. |
| -v | Specify an integer verbosity level. Default is zero, but values of 1 and 2 are currently supported. |

The default verbosity setting is zero, which only prints summary data. The test below has only 11 encrypted packets, wich demonstrates that running multiple tests for randomness takes a lot of time. If time is of the essence, identify specific tests as the most important, such as the Monobit test and one or two others and run only those tests against the data.

~~~~
$ python encrypted.py -f wireshark/fuzz-2010-06-29-8087.pcap -o test.csv

Reading from wireshark/fuzz-2010-06-29-8087.pcap
Writing to test.csv

Total Read time: 12.85062026977539 for 86401 encrypted:11
~~~~

Specifying a verbosity of one identifies the encrypted packets as well as their source and destination.

~~~~
$ python encrypted.py -f wireshark/fuzz-2010-06-29-8087.pcap -o test.csv -v 1


Reading from wireshark/fuzz-2010-06-29-8087.pcap
Writing to test.csv
Encrypted packet at index 1853 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 20584 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 36123 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 39863 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 46962 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 55559 10.4.64.69 ==> 10.4.64.187
Encrypted packet at index 68948 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 75670 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 78957 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 81548 10.4.64.187 ==> 10.4.64.69
Encrypted packet at index 82699 10.4.64.187 ==> 10.4.64.69

Total Read time: 12.936828374862671 for 86401 encrypted:11
~~~~

Increasing the verbosity to two causes the payload length to be printed as well as a full dump of the data for every encrypted packet. Interesting, but not normally desired.


## string_probability.py

This program displays the probability that a set of data of size D contains the same string of length S n times. First consider the help.

~~~~
$ python string_probability.py -h
usage: String Probability [-h] [-s S] [-n N [N ...]] [-d D] [-u {pb,tb,mb,GB,PB,MB,gb,kb,TB,KB}]

Print the probability of String with length S occurring N times in Data of size D.

options:
  -h, --help            show this help message and exit
  -s S                  Length of string S
  -n N [N ...]          The number of occurrences of S
  -d D                  The size of the data
  -u {pb,tb,mb,GB,PB,MB,gb,kb,TB,KB}
                        The unit of the size of data, default is 'GB'.

Supported systems will display a graph.
~~~~

In the example below, 30 GB of data is examined. Three strings of length 4 are found to be repeated 4, 10, and 6 times.

~~~~
$ python string_probability.py -s 4 -n 4 10 6 -d 30
P(1) is 0.415%
P(2) is 1.56%
P(3) is 3.89%
P(4) is 7.29%
P(5) is 10.9%
P(6) is 13.7%
P(7) is 14.6%
P(8) is 13.7%
P(9) is 11.4%
P(10) is 8.58%
Probability of 3 strings occurring (4, 10, 6) times with length:4 = 0.0856%
~~~~

The probability P(n), as shown above, is the probability that a string of length 4 exists exactly n times in 30GB of data. So, the probability that a string of length 4 exists in the data exactly one time is 0.415%.

The probability that three strings will be seen exactly 4, 10, and 6 times is obtained by multiplying the individual probabilities together for 0.415%.

### The Math in string_probability.py

Each 8-bit character is assumed to be random and uniformily distributed.
Probabilities are obtained using the Poisson distribution (see [https://en.wikipedia.org/wiki/Poisson_distribution](https://en.wikipedia.org/wiki/Poisson_distribution)). 

\begin{equation}
2^{-2}
\end{equation}

The string S contain s 8-bit integers so the Rate of S is $r = 2^{-8s}$.

The Data is expressed in a common unit such as GB so it is represented as 
$D2^k$ so 10GB is $10*2^{30}$. 

The expected rate of the string S is $λ=r*t = D2^{k}s^{-8s} = D2^{k-8s}$.

The sample run uses 30GB and a string of length 4 yielding $λ=30*2^{-2}$. 
A poisson library is used to calculate the percentages shown in our sample run.

The Poisson distribution is as follows: 

$P(X=k) = \frac{λ^{k}e^{-λ}}{k!}$

The smallest floating point number in Python is $2.225*10^{-308}$ so values of 
λ greater than 708 will cause $e^{-λ}$ to underflow to 0. 
At what point does this occur? Using the equation for λ above, 
we have that $λ=D2^{k-8s}$ and we want $λ<708$. This implies that
$708 < D2^{k-8s}$. Take log base 2 of each side. So if 
$9.5 \geq log_2(D) + k - 8s$ then there will likely be an underflow to zero.


## Tools by Category


### Internet Tools
- `ciscot7.py` <br/>
This is a tool designed to decrypt insecure (password 7) Cisco passwords.

ciscot7.py supports the following arguments: <br/>

| parameter | Description |
|-----------|-------------|
| -e        | Encrypt Password. |
| -d        | Decrypt Password; this is the default setting. |
| -p        | Password to encrypt / decrypt. |
| -f        | Cisco config file, only for decryption. |
| --help    | Print usage. |

<br/>

This is a simple example of encrypting and decrypting a password provided at the command line.

~~~~
$ python ciscot7.py -e -p MyPassword
Encrypted password: 047612360e325f59060b01
$ python ciscot7.py -d -p 047612360e325f59060b01
Decrypted password: MyPassword
~~~~

### Internally developed
- Topology Reconstruction (kmeans.py)
- Information Graph
- Configuration File Parsing
- Randomness Assessment for ESP Payload data (encryption.ph)

# Acronyms

| Acronym | Description |
|---------|-------------|
| ESP     | IP Encapsulating Security Protocol is designed to provide a mix of security services (encryption) in IPv4 and IPv6. |
| IP      | The Internet Protocol (IP) is the network layer communications protocol in the Internet protocol suite for relaying datagrams across network boundaries. Its routing function enables internetworking, and essentially establishes the Internet. |
| LFSR    | A Linear Feedbak Shift Register is a shift register whose input bit is a linear function of its previous state. |
| NIST    | National Institute of Standards and Technology. |
| PCAP  | Packet Capture files are readable and writable by by wireshark. |

<br/>
