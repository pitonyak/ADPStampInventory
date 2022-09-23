
# Heuristics Program

The primary purpose of the heuristics is to identify anomalys in a PCAP file. 

## Configuration Files

Configuration files are expected to be in the same directory as the executable. The files are referenced without a path.

- eth_types.txt - A list of the valid / expected Ethernet types. Indicates if protocol is valid, MAC or IP is an expected part of the payload.

- ip_protocol_ports.txt - For each protocol, lists of suported ports. Indicates if protocol is valid, MAC or IP is an expected part of the payload. This file is not finalized, especially in that I have not set the columns for entires that may support the IP or MAC to be part of the payload. If no entry found here, value in ip_protocols.txt is used instead.

- ip_protocols.txt - List of supported IP protocols. Indicates if protocol is valid, MAC or IP is an expected part of the payload. This file is not finalized, especially in that I have not set the columns for entires that may support the IP or MAC to be part of the payload.

- ip_types.txt - **[Deprecated]** List of the valid / expected IP protocols. One entry per IP/Port. See ip_protocols.txt and ip_protocol_ports.txt.



Source Files: 

ahocorasickbinary.* - Aho Corasick search. Efficiently searches binary data for multiple MAC or IP (in binary).

bitsetdynamic.* - Used by Aho Corasick. Holds a dynamically sized bitset with various operations.

eth_types.txt - Line based text file containing valid ethernet types. The file contains comments specifying the format. 

crc32_x.* - Opensource code to calculate the 32-bit CRC.

crc_test.cpp - Generate the 32-bit CRC for a list of files. This reads files in 10MB chunks so it can handle large files and it demonstrates how 
to calculate a CRC in chunks.

ethtype.* - Meta data while searching ethernet types such as is a duplicate MAC expected in the payload. Defines the type and the container for a list.

find_macs_and_ips.cpp - Independent program to generate a list of MAC and IPs contained in a PCAP file. This functionality is implemented independently in the main heuristic program.

ipaddresses.cpp - Maintains two lists of IP addresses (in binary). One is IPv4 and one is IPv6. Data is stored in a set so search is based on std::set.

iptype.* - This will hold a single IP Type entry with the values from the ip_types.txt file. This has no even almost been flushed out. I need to add a routine to read the entire file. 

macaddresses.cpp - Maintains a list of MAC addresses (in binary). Data is stored in a set so search is based on std::set.

main.cpp - Main executable. The Makefile creates the executable named "heuristic" by default.

Makefile.dat - original Makefile that I rename to Makefile so I can use make to build. This file should work as is depending on the location of the libpcap include files. On Fedora Linux I installed the libpcap development files and things just worked. If you want to read a json file, download the include files from rapidjson then set the location with a similar line to: INCLUDE:=-I/andrew0/home/andy/Devsrc/Battelle/GreenHornet/git_stuff/rapidjson/include

utilities.* - Pulling utility methods, especially string function / methods to a single location rather than duplicating them.

utilities_test.cpp


To build, copy Makefile.dat to Makefile then you can use the command make to build the heursitic file. Use "make clean" to clean (delete) things. Use "make crc_test" to build the crc_test executable.

The program uses the libpcap library. You must also include the development version. On Ubuntu this means libpcap-dev and on Fedora libpcap-devel.
