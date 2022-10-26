
# ISAKMP Tool

The primary purpose of the ISAKMP tool is to identify devices of interest based on the vendor ID in the ISAKMP packets found in a PCAP file.

## Files

### Configuration

Configuration files are expected to be in the same directory as the executable. The files are referenced without a path.

| Configuration File     | Description |
|------------------------|-------------|
| vendor_match.txt       | Defines the six-byte sequence that will be matched against the vendor IDs in ISAKMP packets. |

The configuration files can be edited in place and the program re-run.

- All configuration files start a line based comment using the '**#**' character.
- Leading and trailing white space is ignored / removed.
- Comments and empty lines are ignored.

### Source Code

The following source files are in the project: <br/>

| Source File(s)         | Description |
|------------------------|-------------|
| isakmp.h               | Contains structures and constants used to parse ISAKMP packets. |
| ip_address_pairs.*     | Maintains set of IP address pairs that where one or both of the addresses identifies a device that has sent ISAKMP packets containing a vendor id of interest. |
| main.cpp               | Main executable. The Makefile creates the executable named "isakmp_vendor" by default. |
| MurmurHash.*           | Public domain hashing code used for hashing items in the ip_address_pairs code. |
| string_util.*          | Utility functions that operate on std::string objects. |

### Executables

The executable files are generated using *make* as mentioned in the **Build** section. <br/>

| Executable File        | Description |
|------------------------|-------------|
| isakmp_vendor          | Main executable program to read PCAP file, identify devices of interest and and filter traffic from these devices into a PCAP output file. |
## Build

Source files are built using *make*.

The ISAKMP tool uses the libpcap library. You must also include the development version. On Ubuntu this means libpcap-dev and on Fedora libpcap-devel. You must have the required libraries installed. Installing these projects using the operating system dependent methods places the required libraries and include files where they are needed; for example, apt-get, apt, dnf, or yum.

Use "make clean" to clean (delete) things. Use make by itself to build all of the executables as listed above.

The build flags (CXXFLAGS) in Makefile are best for the production environment. For development, adding sanitize helps to find memory leaks and other errors and adding the -g flag adds debug info which is helpful when running the program under a debugger such as gdb or lldb.

`CXXFLAGS:=-O3 -fPIC -Werror -Wall -Wextra -fsanitize=undefined,address -fno-sanitize=alignment -g`

These flags are always used. <br/>

| Compiler Flag | Description |
|------------------------|-------------|
| O3     | Optimize the code; more optimizations than O2. |
| fPIC   | Emit position-independent code. |
| Werror | Make all warnings errors. |
| Wall   | Turn on all warnings (well almost all). |
| Wextra | Turn on a few more not covered with -Wall (see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html); and there are even more warnings still not included with this. |

Only use this in development because they affect runtime, sometimes by a significant amount. <br/>

| Compiler Flag          | Description |
|------------------------|-------------|
| fsanitize=address      | Enable the AddressSanitizer (fast memory error detector). |
| fsanitize=undefined    | UndefinedBehaviorSanitizer. |
| fno-sanitize=alignment | Prevents complaints while accessing types at improper boundaries; as we do while pulling things such as headers from the binary data in a packet. Only a problem because of the *-fsantize* flag. |
| g                      | Adds debugging information to the executable. |

## Running The ISAKMP Tool

The ISAKMP tool analyzes PCAP files looking for ISAKMP packets whose vendor ID payloads contain a byte sequence that matches the one defined in the vendor_match.txt configuration file. Packets that have been sent or received from the source and destination IP addresses associated with the ISAKMP packets are then output into another PCAP file.

Running the program with the `--help` or `-h` option prints usage information.

~~~~
$ ./iskamp_vendor --help
Filter packets from devices sending ISAKMP packets containing a particular vendor id.

Usage: ./isakmp_vendor [options] -r <infile> -w <outfile>

  -r, --read-file=FILE    Set input pcap filename
  -w, --write-file=FILE   Set output pcap filename
  -m, --match-file=FILE   Set filename to read vendor id byte sequence to match against ISAKMP packets.
                          If not specified, vendor_match.txt will be used from the current directory.
  -v, --verbose           Display verbose output
  -h, --help              Display this help and exit
~~~~

The program requires two options to be specifed. The first one is `--read-file` (or `-r`), which specifies the input PCAP file to analyze. The second one is `--write-file` (or `-w`), which sets the name of the output PCAP file in which filtered packets will be written.

The `--match-file` (or `-m`) option may be used to override the path to the file which defines the vendor ID byte sequence to match against in ISAKMP packets. By default this is assumed to be "vendor_match.txt" which should be in the same directory as the isakmp_vendor executable.

The `--verbose` (or `-v`) option prints additional information about packets found in the PCAP file as it is being processed. This may be useful for debugging purposes but it is not recommended to be used with large PCAP files.
