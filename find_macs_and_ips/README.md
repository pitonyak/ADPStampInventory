
Files: 

main.cpp - Main executable

Makefile.dat - original Makefile that I rename to Makefile so I can use make to build. This file should work as is depending on the location of the libpcap include files. On Fedora Linux I installed the libpcap development files and things just worked. If you want to read a json file, download the include files from rapidjson then set the location with a similar line to: INCLUDE:=-I/andrew0/home/andy/Devsrc/Battelle/GreenHornet/git_stuff/rapidjson/include

eth_types.txt - Line based text file containing valid ethernet types. The file contains comments specifying the format. 

ether_type_final.json - JSON file containing decimal type and a comment. 

read_json.cpp - Example on how to read the existing ether_type_final.json file. I expect the JSON file to change if we choose to use the format.

RapidJSON github project to read json. 

https://github.com/Tencent/rapidjson/



