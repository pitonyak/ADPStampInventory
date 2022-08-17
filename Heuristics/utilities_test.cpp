
// gcc utilities.cpp utilities_test.cpp -lstdc++

#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include "utilities.h"

int test_search() {
	//find_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len);

	u_int8_t ui1=0, ui2=0;
	if (find_match((const u_int8_t*)&ui1, (u_int32_t)1, (const u_int8_t*)&ui2, (u_int32_t)1)) {
		std::cout << "find_match of length 1 passed" << std::endl;
	} else {
		std::cout << "find_match of length 1 failed" << std::endl;
	}

	if (reverse_match((const u_int8_t*)&ui1, (u_int32_t)1, (const u_int8_t*)&ui2, (u_int32_t)1)) {
		std::cout << "reverse_match of length 1 passed" << std::endl;
	} else {
		std::cout << "reverse_match of length 1 failed" << std::endl;
	}

	return 0;
}


int main(int argc, char **argv) {

  test_search();
  return 0;
}
