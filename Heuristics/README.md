
Files: 

main.cpp - Main executable

Makefile.dat - original Makefile that I rename to Makefile so I can use make to build. This file should work as is depending on the location of the libpcap include files. On Fedora Linux I installed the libpcap development files and things just worked. If you want to read a json file, download the include files from rapidjson then set the location with a similar line to: INCLUDE:=-I/andrew0/home/andy/Devsrc/Battelle/GreenHornet/git_stuff/rapidjson/include

eth_types.txt - Line based text file containing valid ethernet types. The file contains comments specifying the format. 

ip_types.txt - A list of the valid / expected IP types. This file is not finalized, especially in that I have not set the columns for entires that may support the IP or MAC to be part of the payload. 

iptype.* - This will hold a single IP Type entry with the values from the ip_types.txt file. This has no even almost been flushed out. I need to add a routine to read the entire file. 

utilities.* - Pulling utility methods, especially string function / methods to a single location rather than duplicating them.


