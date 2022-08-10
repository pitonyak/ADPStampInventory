
/**
 * Simple test program for the CRC.  
 * Calculates the CRC of the heuristics binary.
 * 
 * Compile using: 
 * gcc crc_test.cpp crc32_x.cpp -lstdc++
 * 
 **/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <string>

#include "crc32_x.h"


int main(int argc, char **argv) {

	if (argc < 2) {
		std::cout << "Enter one or more filenames to print the 32-bit CRC for each file." << std::endl << std::endl;
		return 0;
	}

	for (int i = 1; i<argc; ++i) {
		size_t length;
		char * buffer;

		std::ifstream is;
		is.open (argv[i], std::ios::binary );
		if (!is.is_open() || !is.good()) {
			std::cout << "Failed to open file: " << argv[i] << std::endl;
			continue;
		}

        // tellg() returns a (long long), but length is an unsigned int.
        // I could use a static_cast, but there are probably bigger 
        // problems if the file is long enough for that to be an issue.
        
		// get length of file:
		is.seekg (0, std::ios::end);
		length = is.tellg();
		is.seekg (0, std::ios::beg);
		// allocate memory:
		buffer = new char [length];
		// read data as a block:
		is.read (buffer,length);
		is.close();

		uint32_t crc_1 = crc32(0, buffer, length);
		delete[] buffer;

		std::cout << std::hex << crc_1 << "        " << argv[i] << std::endl;
	}
	return 0;
}