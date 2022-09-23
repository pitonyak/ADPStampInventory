
# Heuristics Program

The primary purpose of the heuristics is to identify anomalys in a PCAP file.

## Files

### Configuration

Configuration files are expected to be in the same directory as the executable. The files are referenced without a path.

| Configuration File     | Description |
|------------------------|-------------|
| eth_types.txt          | A list of the valid / expected Ethernet types. Indicates if protocol is valid, MAC or IP is an expected part of the payload. |
| ip_protocol_ports.txt  | For each protocol, lists of suported ports. Indicates if protocol is valid, MAC or IP is an expected part of the payload. This file is not finalized, especially in that I have not set the columns for entires that may support the IP or MAC to be part of the payload. If no entry found here, value in ip_protocols.txt is used instead. |
| ip_protocols.txt       | List of supported IP protocols. Indicates if protocol is valid, MAC or IP is an expected part of the payload. This file is not finalized, especially in that I have not set the columns for entires that may support the IP or MAC to be part of the payload. |
| ip_types.txt           | **[Deprecated]** List of the valid / expected IP protocols. One entry per IP/Port. See ip_protocols.txt and ip_protocol_ports.txt. |

The configuration files can be edited in place and the program re-run. 

### Source Code

Source files are built using *make*.

| Source File(s)         | Description |
|------------------------|-------------|
| ahocorasickbinary.*    | Aho Corasick search. Efficiently searches binary data for multiple MAC or IP (in binary). |
| bitsetdynamic.*        | Used by Aho Corasick. Holds a dynamically sized bitset with various operations. |
| crc32_x.*              | Opensource code to calculate the 32-bit CRC. |
| crc_test.cpp           | Generate the 32|bit CRC for a list of files. This reads files in 10MB chunks so it can handle large files and it demonstrates how to calculate a CRC in chunks. |
| ethtype.*              | Meta data while searching ethernet types such as is a duplicate MAC expected in the payload. Defines the type and the container for a list. |
| find_macs_and_ips.cpp  | Independent program to generate a list of MAC and IPs contained in a PCAP file. This functionality is implemented independently in the main heuristic program. |
| ipaddresses.cpp        | Maintains two lists of IP addresses (in binary). One is IPv4 and one is IPv6. Data is stored in a set so search is based on std::set. |
| iptype.*               | This will hold a single IP Type entry with the values from the ip_types.txt file. This has no even almost been flushed out. I need to add a routine to read the entire file.  |
| macaddresses.cpp       | Maintains a list of MAC addresses (in binary). Data is stored in a set so search is based on std::set. |
| main.cpp               | Main executable. The Makefile creates the executable named "heuristic" by default. |
| utilities.*            | Pulling utility methods, especially string function / methods to a single location rather than duplicating them. |
| utilities_test.cpp     | Test program for code in the utilities. |

### Executables

The executable files are generated using *make* as mentioned in the **Build** section. 

| Executable File        | Description |
|------------------------|-------------|
| crc_test               | Generat a 32-bit CRC for a file or list of files. Used to test the CRC code. |
| find_macs_and_ips      | Independent program to generate a list of MAC and IPs contained in a PCAP file. This functionality is implemented independently in the main heuristic program. |
| heuristics             | Main executable program to read a PCAP and generate an anomaly PCAP file. |
| utilities_test         | Test the utility code such as searching. |

## Build

The heuristic program uses the libpcap library. You must also include the development version. On Ubuntu this means libpcap-dev and on Fedora libpcap-devel. You must have the required libraries installed. 

Copy Makefile.dat to Makefile to build. This file should work as is depending on the location of the libpcap include files. On Fedora Linux I installed the libpcap development files and things just worked. If you want to read a json file, download the include files from rapidjson then set the location with a similar line to: INCLUDE:=-I/andrew0/home/andy/Devsrc/Battelle/GreenHornet/git_stuff/rapidjson/include

Use "make clean" to clean (delete) things. Use "make crc_test" to build the crc_test executable. Use make by itself to build all of the executables as listed above.

The build flags (CXXFLAGS) in Makefile.dat are best for the production environment. For development, adding sanitize helps to find memory leaks and other errors. 

`CXXFLAGS:=-O3 -fPIC -Werror -Wall -Wextra -fsanitize=undefined,address -fno-sanitize=alignment`

These flags are always used. 

| Compiler Flag | Description |
|------------------------|-------------|
| O3     | Optimize the code; more optimizations than O2. |
| fPIC   | Emit position-independent code. |
| Werror | Make all warnings errors. |
| Wall   | Turn on all warnings (well almost all). |
| Wextra | Turn on a few more not covered with -Wall (see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html); and there are even more warnings still not included with this. |

Only use this in development because they affect runtime, sometimes by a significant amount. 

| Compiler Flag          | Description |
|------------------------|-------------|
| fsanitize=address      | Enable the AddressSanitizer (fast memory error detector). |
| fsanitize=undefined    | UndefinedBehaviorSanitizer. |
| fno-sanitize=alignment | Prevents complaints while accessing types at improper boundaries; as we do while pulling things such as headers from the compacted binary in a pcket. Only a problem because of the *-fsantize* flag. |

As a simple test, the crc_test program run against a 3.6G took about 16 seconds using sanitize and 7.5 seconds without.

## Running

### utilities_test

The utitilities test perform the following tests:

- Tests for search tests the forwards, backwards, and Aho Corasick tests. 
- Tests for the bitset tests the dynamic bitset class. 
- For IP tests, the latest files are read, and this demonstrates that the configuration files are where they should be. 
- A test is also performed to make sure that comparisons between the binary data and an interger works. 

{
[Heuristics]$ ./utilities_test


passed:34 failed:0 for search

passed:8329 failed:0 for bitset testing
Processing IP protocol 6 for known ports in configuration file.
Processing IP protocol 17 for known ports in configuration file.

passed:15 failed:0 for Ethernet and IP types

PASS: Set ipHeader.ip_p to 6 and compare to IPPROTO_TCP
}

### crc_test


[andy@DESKTOP-6U3CUIF Heuristics]$ ./crc_test  crc_test utilities_test
ec601980        crc_test
7ab0ca83        utilities_test
[andy@DESKTOP-6U3CUIF Heuristics]$ crc32 crc_test utilities_test
ec601980	crc_test
7ab0ca83	utilities_test



### find_macs_and_ips



## Running The Heuristic




