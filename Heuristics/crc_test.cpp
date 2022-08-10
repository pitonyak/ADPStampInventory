
/**
 * Simple test program for the CRC.  
 * Calculates the CRC of the heuristics binary.
 * 
 * Compile using (creates a.out): 
 * gcc crc_test.cpp crc32_x.cpp -lstdc++
 * 
 * You can also use "make crc_test" to build crc_test rather than a.out.
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

    // Set the buffer to 10MB.
	size_t max_buffer = 1024 * 1024 * 10;
	char * buffer = new char[max_buffer];

	for (int i = 1; i<argc; ++i) {
		std::ifstream is;
		is.open (argv[i], std::ios::binary );
		if (!is.is_open() || !is.good()) {
			std::cout << "Failed to open file: " << argv[i] << std::endl;
			continue;
		}

		// read data as a block:
		is.read(buffer, max_buffer);
		uint32_t the_crc = 0;
		while (is.gcount() > 0) {
			the_crc = crc32(the_crc, buffer, is.gcount());
			is.read(buffer, max_buffer);
		}
		is.close();

		std::cout << std::hex << the_crc << "        " << argv[i] << std::endl;
	}
	delete[] buffer;
	return 0;
}