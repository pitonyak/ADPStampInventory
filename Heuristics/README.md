
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

- All configuration files start a line based comment using the '**#**' character.
- Leading and trailing white space is ignored / removed.
- Comments and empty lines are ignored.
- All white space in a line is converted to a single space character.
- Multiple spaces are always converted to a single space.

#### eth_types.txt

The columns in order are: 

1. Ethernet type expressed in HEX. A range may be used by separating the values with a minus sign. No space is allowed in the ethernet type.
2. Valid (1) or invalid (0). Invalid types are added to the anomaly list when seen.
3. IPv6 is unused, but indicates if the type is expected for IPv6. This may be removed in the future.
4. If 1, the payload may contain the source or destination IP as part of the payload so do not look for an IP in the payload.
5. If 1, the payload may contain the source or destination MAC as part of the payload so do not look for a MAC in the payload.
6. The rest of the line is considered a comment to remind the person editing the file the purpose of the ethernet type.

An excerpt from the eth_types.txt file is shown below:

~~~~
  #Hex	Valid  IPv6 IPs MACs Description
  0000-05DC	1	0	0	0	 IEEE802.3 Length Field (0.:1500.)
  0101-01FF	1	0	0	0	 Experimental
  0200		1	0	0	0	 Xerox PUP (conflicts with 802.3 Length Field range) (see 0A00)
  0201		1	0	0	0	 Xerox PUP Address Translation (conflicts ...) (see 0A01)
  0400		1	0	0	0	 Nixdorf (conflicts with 802.3 Length Field)
  0600		1	0	0	0	 Xerox NS IDP
  0601		1	0	0	0	 XNS Address Translation (3Mb only)
  0660		1	0	0	0	 DLOG
  0661		1	0	0	0	 DLOG
~~~~

#### ip_protocols.txt

This file outlines specific supported IP protocols, which means that the Ethernet type ETHERTYPE_IP (0x0800). IPv6 is not yet supported. This file has a list of Internet Protocol Types and an indicator if the payload having a duplicate IP address or MAC address is considered normal. This is used in conjunction with the ip_protocol_ports.txt file. 

The columns in order are: 

1. Internet Protocol Type in decimal. A range may be used by separating the values with a minus sign. No space is allowed.
2. Valid (1) or invalid (0). Invalid types are added to the anomaly list when seen.
3. If 1, the payload may contain the source or destination IP as part of the payload so do not look for an IP in the payload.
4. If 1, the payload may contain the source or destination MAC as part of the payload so do not look for a MAC in the payload.
5. The rest of the line is considered a comment to remind the person editing the file the purpose of the ethernet type.

An excerpt from the ip_protocols.txt file is shown below.

~~~~
 #  OK IPs MACs  Description
 0	1	0	0    IPv6 Hop-by-Hop Option (HOPOPT)
 1	1	1	0    Internet Control Mess (ICMP)
 2	1	0	0    Internet Group Management (IGMP)
 3	1	0	0    Gateway-to-Gateway (GGP)
 4	1	0	0    IPv4 encapsulation
~~~~

#### ip_protocol_ports.txt

This file allows for a list of ports along with whether or not the MAC or IP is expected to have the IP Address or MAC address included in the payload. If a specific port is not included in this file, the value in the ip_protocols.txt file is used instead. So, if you check to see if port 1 expects a repeated IP Address and port 1 is not listed for a specific protocol, the *default* value for that protocol is used from the ip_protocols.txt file. Only two protocols are currently supported in the code to pull the ports, TCP and UDP. The file may list ports for other protocols, but this is not supported in the code. Also, IPv6 is not supported to check ports. 

The Internet Protocol Type is listed on a line by itself such as "PROTOCOL=6", where the protocol is a decimal number.

Each port for that protocol has a line with the following columns:

1. Port number in decimal. A range is not supported.
2. Valid (1) or invalid (0). Invalid ports are added to the anomaly list when seen.
3. If 1, the payload may contain the source or destination IP as part of the payload so do not look for an IP in the payload.
4. If 1, the payload may contain the source or destination MAC as part of the payload so do not look for a MAC in the payload.
5. The rest of the line is considered a comment to remind the person editing the file the purpose of the ethernet type.

An excerpt from the ip_protocol_port.txt file is shown below.

~~~~
 PROTOCOL=6
 #PORT	OK	IPs	MACs	Description
 161		1	1	1		SNMP (UDP)
 514		1	1	1		System Logs
 53		1	1	1		Domain Name Service (DNS)

 # Protocol UDP = 17
 PROTOCOL=17
 #PORT	OK	IPs	MACs	Description
 161		1	1	1		SNMP (UDP)
 514		1	1	1		System Logs
 53		1	1	1		Domain Name Service (DNS)
~~~~

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

~~~~
	$ ./utilities_test

	passed:34 failed:0 for search

	passed:8329 failed:0 for bitset testing
	Processing IP protocol 6 for known ports in configuration file.
	Processing IP protocol 17 for known ports in configuration file.

	passed:15 failed:0 for Ethernet and IP types

	PASS: Set ipHeader.ip_p to 6 and compare to IPPROTO_TCP
~~~~

### crc_test

The CRC-32 was added to allow for testing some apsepcts of the data. The crc_test program acts the same as the crc32 program on linux. Below, both programs are run against two files and the same 32-bit CRC is generated. 

{

	$ ./crc_test  crc_test utilities_test
	ec601980        crc_test
	7ab0ca83        utilities_test

	$ crc32 crc_test utilities_test
	ec601980	crc_test
	7ab0ca83	utilities_test

}


### find_macs_and_ips

This program generates a lsit of IP addresses and MAC addresses used in a PCAP file. Running with no parameters generates help. 

~~~~
	$ ./find_macs_and_ips 
	Usage:
	-p <path to IP output filename, default 'ip_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all IP addresses found in the input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.
	-m <path to MAC output filename, default 'mac_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all Ethernet MAC addresses input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.
	-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses
~~~~

This functionality also exists in the Heuristic program automatically so is not usually needed.

## Running The Heuristic




