
// gcc utilities.cpp utilities_test.cpp -lstdc++

#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include "utilities.h"

typedef bool (*SearchFunc)(const u_int8_t*, u_int32_t, const u_int8_t*, u_int32_t);

void dump_array(const u_int8_t* x, u_int32_t n) {
	if (x == nullptr) {
		std::cout << "nullptr";
	} else {
		for (u_int32_t i = 0; i<n; ++i) {
			std::cout << std::hex << (unsigned) x[i] << " " << std::dec;
		}
	}
}

int test_search() {
	//bool find_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len);
    //bool reverse_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len);

    SearchFunc searches[] = {&find_match, &reverse_match};

   	u_int8_t ui1=0, ui2=0, ui3=1;

   	u_int8_t ui4[2] = {1, 1};
   	u_int8_t ui5[2] = {0, 1};
    u_int8_t ui6[2] = {1, 0};

    const std::string search_name[] = {"find_match", "reverse_match"};
    const std::string search_type[] = {"match s len=1, data len=1", "no match s len=1, data len=1", 
    	"no match s len=0, data len=1", "no match s len=1, data len=0",
        "no match s=null, data len=1", "no match s len=1, data=null",
        "match s len=2, data len=2", "match s len=2, data len=2", "no match s len=2, data len=2", "no match s len=2, data len=2", 
        "match s len=1, data len=2", "match s len=1, data len=2", "no match s len=1, data len=2", 
    };
    const u_int8_t* data[] = {&ui1, &ui2, &ui1, &ui3, &ui1, &ui2, &ui1, &ui2, nullptr, &ui2, &ui1, nullptr,
    	ui4, ui4, ui5, ui5, ui5, ui4, ui6, ui4, 
      &ui1, ui5, &ui1, ui6, &ui1, ui4, 
    };
    u_int32_t lengths[] = {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1,
    	2, 2, 2, 2, 2, 2, 2, 2,
      1, 2, 1, 2, 1, 2,
    };
    bool should_find[] = {true, false, false, false, false, false,
    	true, true, false, false,
      true, true, false,
    };

    int num_passed = 0;
    int num_failed = 0;
    int num_funcs = sizeof(searches) / sizeof(searches[0]);
    int num_tests = sizeof(should_find) / sizeof(should_find[0]);

	std::string pass = "Pass ";
	std::string fail = "FAIL ";
    for (int i_func = 0; i_func < num_funcs; ++i_func) {
    	for (int i_test = 0; i_test < num_tests; ++i_test) {
    		if ((*searches[i_func])(data[2*i_test], lengths[2*i_test], data[2*i_test+1], lengths[2*i_test+1]) == should_find[i_test]) {
    			std::cout << pass << search_name[i_func] << " " << search_type[i_test] << std::endl;
    			++num_passed;
    		} else {
    			std::cout << fail << search_name[i_func] << " " << search_type[i_test] << std::endl;
    			std::cout << "  (1) = ";
    			dump_array(data[2*i_test], lengths[2*i_test]);
    			std::cout << std::endl << "  (2) = ";
    			dump_array(data[2*i_test + 1], lengths[2*i_test + 1]);
    			std::cout << std::endl;
    			++num_failed;
    		}
    	}
    }

    std::cout << std::endl << "passed:" << num_passed << " failed:" << num_failed << std::endl;

	return 0;
}


int main(int argc, char **argv) {

  test_search();
  return 0;
}
