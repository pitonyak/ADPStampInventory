
// gcc utilities.cpp utilities_test.cpp -lstdc++

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include "ahocorasickbinary.h"
#include "bitsetdynamic.h"
#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "MurmurHash3.h"
#include "utilities.h"

typedef bool (*SearchFunc)(const uint8_t*, uint32_t, const uint8_t*, uint32_t);

void dump_array(const uint8_t* x, uint32_t n) {
	if (x == nullptr) {
		std::cout << "nullptr";
	} else {
		for (uint32_t i = 0; i<n; ++i) {
			std::cout << std::hex << (unsigned) x[i] << " " << std::dec;
		}
	}
}

void print_results(int num_passed, int num_failed, const std::string& message) {
  std::cout << std::endl;
  if (num_passed == 0 && num_failed == 0) {
    std::cout << "Nothing passed or failed " << message << std::endl;
  } else {
    if (num_failed != 0) std::cout << "*** FAILED:" << num_failed << " ";
    if (num_passed != 0) std::cout << "passed:" << num_passed << " ";
  }
  std::cout << message << std::endl;
}

int test_aho_overlap() {
  AhoCorasickBinary acb;
  std::vector<uint8_t *> words;
  std::vector<int> word_lengths;

  uint8_t word_0[] = {65, 65, 65}; // AAA
  uint8_t word_1[] = {66, 65, 66}; // BAB
  words.push_back(word_0);
  words.push_back(word_1);
  word_lengths.push_back(sizeof(word_0)/sizeof(*word_0));
  word_lengths.push_back(sizeof(word_1)/sizeof(*word_1));
  acb.buildMatchingMachine(words, word_lengths);

  // Word 0 has matches that end at 11 and 12
  // Word 1 has matches that end at 6 and 8
  // AACCBABABAAAAC
  uint8_t search_1[] = {65, 65, 67, 67, 66, 65, 66, 65, 66, 65, 65, 65, 65, 67};

  // Should find four words, both overlapping.
  std::map<int, std::set<int> > matches = acb.findAllMatches(search_1, sizeof(search_1)/sizeof(*search_1));
  
  if (matches.size() != 2) {
    std::cout << "Expected to find 2 strings not " << matches.size() << "." << std::endl;
    return 1;
  }

  if (acb.countMatches(matches) != 4) {
    std::cout << "Expected to find 4 matches total, not " << acb.countMatches(matches) << "." << std::endl;
    return 1;
  }

  std::map<int, std::set<int> >::iterator it;
  // Look at word 1
  it = matches.find(1);
  if (it == matches.end()) {
    std::cout << "Did not find matches for keyword 1 (BAB)." << std::endl;
    return 1;
  } else if (it->second.size() != 2 || it->second.find(6) == it->second.end() || it->second.find(8) == it->second.end()) {
    std::cout << "Did not find two overlap matches for keyword 1 (BAB)." << std::endl;
    return 1;
  }

  it = matches.find(0);
  if (it == matches.end()) {
    std::cout << "Did not find matches for keyword 0 (AAA)." << std::endl;
    return 1;
  } else if (it->second.size() != 2 || it->second.find(11) == it->second.end() || it->second.find(12) == it->second.end()) {
    std::cout << "Did not find two overlap matches for keyword 0 (AAA)." << std::endl;
    return 1;
  }

  std::cout << "Aho-Corasick Correctly found overlapping matches." << std::endl;
  return 0;
}

int test_search() {

  SearchFunc searches[] = {&find_match, &reverse_match};

 	uint8_t ui1=0, ui2=0, ui3=1;

 	uint8_t ui4[] = {1, 1};
 	uint8_t ui5[] = {0, 1};
  uint8_t ui6[] = {1, 0};
  uint8_t ui7[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1};
  uint8_t ui8[] = {1, 0, 0, 0, 1};

  const std::string search_name[] = {"find_match", "reverse_match"};
  const std::string search_type[] = {"match s len=1, data len=1", "no match s len=1, data len=1", 
  	"no match s len=0, data len=1", "no match s len=1, data len=0",
      "no match s=null, data len=1", "no match s len=1, data=null",
      "match s len=2, data len=2", "match s len=2, data len=2", "no match s len=2, data len=2", "no match s len=2, data len=2", 
      "match s len=1, data len=2", "match s len=1, data len=2", "no match s len=1, data len=2", 
      "match s len=5, data len=?",
  };
  const uint8_t* data[] = {&ui1, &ui2, &ui1, &ui3, &ui1, &ui2, &ui1, &ui2, nullptr, &ui2, &ui1, nullptr,
  	ui4, ui4, ui5, ui5, ui5, ui4, ui6, ui4, 
    &ui1, ui5, &ui1, ui6, &ui1, ui4, 
    ui8, ui7,
  };
  uint32_t lengths[] = {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1,
  	2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 1, 2, 1, 2,
    sizeof(ui8)/sizeof(ui8[0]), sizeof(ui7)/sizeof(ui7[0])
  };
  bool should_find[] = {true, false, false, false, false, false,
  	true, true, false, false,
    true, true, false,
    true,
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
  			//std::cout << pass << search_name[i_func] << " " << search_type[i_test] << std::endl;
  			++num_passed;
  		} else {
        {
          std::cout << fail << search_name[i_func] << " " << search_type[i_test] << std::endl;
          std::cout << "  (1) = ";
          dump_array(data[2*i_test], lengths[2*i_test]);
          std::cout << std::endl << "  (2) = ";
          dump_array(data[2*i_test + 1], lengths[2*i_test + 1]);
          std::cout << std::endl;
        }
  			++num_failed;
  		}
  	}
  }

  AhoCorasickBinary acb;
  std::vector<uint8_t *> words;
  std::vector<int> word_lengths;

  uint8_t word_0[] = {65};         // A
  uint8_t word_1[] = {65, 66};     // AB
  uint8_t word_2[] = {66, 65, 66}; // BAB
  uint8_t word_3[] = {66, 67};     // BC
  uint8_t word_4[] = {66, 67, 65}; // BCA
  uint8_t word_5[] = {67};         // C
  uint8_t word_6[] = {67, 65, 65}; // CAA
  uint8_t search_1[] = {65, 66, 67, 67, 65, 66};
  uint8_t search_2[] = {};
  uint8_t search_3[] = {66};
  uint8_t search_4[] = {0, 66, 66, 66, 90, 255, 10};

  if (acb.getNumWords() != 0 || acb.getAlphabetSize() != 256 || acb.getMaxStates() != 0) {
    std::cout << "AhoCorasickBinary object did not initialize correctly on creation." << std::endl;
    ++num_failed;
  } else {
    //std::cout << "AhoCorasickBinary object initialized correctly." << std::endl;
  }

  acb.buildMatchingMachine(words, word_lengths);
  if (acb.getNumWords() != 0 || acb.getAlphabetSize() != 256 || acb.getMaxStates() != 0) {
    std::cout << "AhoCorasickBinary machine initialized incorrectly when sent an empty vetor." << std::endl;
    ++num_failed;
  } else {
    //std::cout << "AhoCorasickBinary machine initialized correctly with empty vector." << std::endl;
  }

  words.push_back(word_0);
  words.push_back(word_1);
  words.push_back(word_2);
  words.push_back(word_3);
  words.push_back(word_4);
  words.push_back(word_5);
  words.push_back(word_6);
  word_lengths.push_back(sizeof(word_0)/sizeof(*word_0));
  word_lengths.push_back(sizeof(word_1)/sizeof(*word_1));
  word_lengths.push_back(sizeof(word_2)/sizeof(*word_2));
  word_lengths.push_back(sizeof(word_3)/sizeof(*word_3));
  word_lengths.push_back(sizeof(word_4)/sizeof(*word_4));
  word_lengths.push_back(sizeof(word_5)/sizeof(*word_5));
  word_lengths.push_back(sizeof(word_6)/sizeof(*word_6));
  acb.buildMatchingMachine(words, word_lengths);

  if (acb.getNumWords() != 7 || acb.getAlphabetSize() != 256 || acb.getMaxStates() != 16) {
    std::cout << "AhoCorasickBinary machine built incorrectly." << std::endl;
    ++num_failed;
  } else {
    //std::cout << "AhoCorasickBinary machine initialized correctly with non-empty vector." << std::endl;
  }

  std::map<int, std::set<int> > matches = acb.findAllMatches(search_1, sizeof(search_1) / sizeof(*search_1));
  std::map<int, std::set<int> >::iterator it;

  bool test_passed = true;

  if (matches.size() != 4) {
    test_passed = false;
    std::cout << "Expected to find 4 of the strings not " << matches.size() << "." << std::endl;
  } else if (matches.find(0) == matches.end() || matches.find(1) == matches.end() || matches.find(3) == matches.end() || matches.find(5) == matches.end()) {
    it = matches.find(0);
    if (it == matches.end()) {
      test_passed = false;
      std::cout << "Did not find matches for keyword 0." << std::endl;
    } else if (it->second.size() != 2 || it->second.find(0) == it->second.end() || it->second.find(4) == it->second.end()) {
      test_passed = false;
      std::cout << "Did not find the correct matches for keyword 0." << std::endl;
    }
    it = matches.find(1);
    if (it == matches.end()) {
      test_passed = false;
      std::cout << "Did not find matches for keyword 1." << std::endl;
    } else if (it->second.size() != 2 || it->second.find(1) == it->second.end() || it->second.find(5) == it->second.end()) {
      test_passed = false;
      std::cout << "Did not find the correct matches for keyword 1." << std::endl;
    }
    it = matches.find(3);
    if (it == matches.end()) {
      test_passed = false;
      std::cout << "Did not find matches for keyword 0." << std::endl;
    } else if (it->second.size() != 1 || it->second.find(2) == it->second.end()) {
      test_passed = false;
      std::cout << "Did not find the correct matches for keyword 3." << std::endl;
    }
    it = matches.find(5);
    if (it == matches.end()) {
      test_passed = false;
      std::cout << "Did not find matches for keyword 0." << std::endl;
    } else if (it->second.size() != 2 || it->second.find(2) == it->second.end() || it->second.find(3) == it->second.end()) {
      test_passed = false;
      std::cout << "Did not find the correct matches for keyword 5." << std::endl;
    }
  }
  if (test_passed) {
    //std::cout << "AhoCorasickBinary::findAllMatches passed." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findAllMatches failed." << std::endl;
    ++num_failed;
  }
  
  if (acb.findFirstMatch(search_2, 0) == -1) {
    //std::cout << "AhoCorasickBinary::findFirstMatch passed with empty data." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findFirstMatch failed with empty data." << std::endl;
    ++num_failed;
  }

  if (acb.findFirstMatch(nullptr, 0) == -1) {
    //std::cout << "AhoCorasickBinary::findFirstMatch passed with null data." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findFirstMatch failed with null data." << std::endl;
    ++num_failed;
  }

  if (acb.findFirstMatch(search_3, sizeof(search_3) / sizeof(*search_3)) == -1) {
    //std::cout << "AhoCorasickBinary::findFirstMatch passed with no match and data len 1." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findFirstMatch failed with with no match and data len 1." << std::endl;
    ++num_failed;
  }

  if (acb.findFirstMatch(search_4, sizeof(search_4) / sizeof(*search_4)) == -1) {
    //std::cout << "AhoCorasickBinary::findFirstMatch passed with no match and data len 7." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findFirstMatch failed with with no match and data len 7." << std::endl;
    ++num_failed;
  }

  if (acb.findFirstMatch(search_1, sizeof(search_1) / sizeof(*search_1)) != -1) {
    //std::cout << "AhoCorasickBinary::findFirstMatch passed with a match and data len 6." << std::endl;
    ++num_passed;
  } else {
    std::cout << "AhoCorasickBinary::findFirstMatch failed with with a match and data len 6." << std::endl;
    ++num_failed;
  }
  print_results(num_passed, num_failed, "for search");

	return num_failed;
}


int test_bits() {
  BitsetDynamic b1;
  int num_failed = 0;
  int num_passed = 0;
  for (std::size_t i=63; i<65; ++i) {
    b1.resetSize(i);
    if (b1.any()) {
      std::cout << "Error, any() failed with all 0 for bitset with size " << i << std::endl;
      ++num_failed;
    } else {
      ++num_passed;
    }
    if (b1.all() && (i != 0)) {
      std::cout << "Error, all() failed with all 0 for bitset with size " << i << std::endl;
      ++num_failed;
    } else {
      ++num_passed;
    }
    if (!b1.none()) {
      std::cout << "Error, none() failed with all 0 for bitset with size " << i << std::endl;
      ++num_failed;
    } else {
      ++num_passed;
    }
    if (i > 0) {
      b1.setAllBits();
      if (b1.count() != i) {
        std::cout << "Error, setAllBits() failed with all bits set for bitset with size " << i << std::endl;
        ++num_failed;
      } else {
        ++num_passed;
      }

      b1.clearAllBits();
      if (b1.count() != 0) {
        std::cout << "Error, clearAllBits() failed with no bits set for bitset with size " << i << std::endl;
        ++num_failed;
      } else {
        ++num_passed;
      }
      b1.setAllBits();
      for (std::size_t j=0; j<i; ++j) {
        b1.setBit(j, false);
        for (std::size_t k=0; k<i; ++k) {
          if (!(b1.at(k) || j == k)) {
            std::cout << "at function failed" << std::endl;
            ++num_failed;
          } else {
            ++num_passed;
          } 
        }
        if (b1.count() != i-1) {
          std::cout << "Error, count() failed with 1 bit set at " << j << " for bitset with size " << i << std::endl;
          ++num_failed;
        } else {
          ++num_passed;
        }
        b1.setBit(j, true);
        if (b1.count() != i) {
          std::cout << "Error, count() failed with no bits set for bitset with size " << i << std::endl;
          ++num_failed;
        } else {
          ++num_passed;
        }
      }
    }
  }
  print_results(num_passed, num_failed, "for bitset");

  return num_failed;
}

int test_ip_mac() {
  int num_failed = 0;
  int num_passed = 0;

  //std::string ip_fname = "ip_types.txt";
  std::string eth_fname = "eth_types.txt";
  std::string new_ip_fname = "ip_protocols.txt";
  std::string new_ip_ports_fname = "ip_protocol_ports.txt";

  struct stat filestat;
  if(stat(new_ip_fname.c_str(), &filestat) == 0){
    IPTypes ip_types;
    //ip_types.readProtocols(ip_fname, true, 10);
    ip_types.readProtocols(new_ip_fname, false, 10);
    ip_types.readProtocolPorts(new_ip_ports_fname);

    // This assumes a few things that may not be true because the idea is that the file is read!
    if (!ip_types.hasType(6, 161)) {
      ++num_failed;
      std::cerr << "Incorrectly states does not have IP type 6 port 161" << std::endl;
    } else {
      ++num_passed;
    }
    if (ip_types.hasType(6, 1)) {
      ++num_failed;
      std::cerr << "Incorrectly states has IP type 6 port 1" << std::endl;
    } else {
      ++num_passed;
    }
    if (!ip_types.hasType(6, -1)) {
      ++num_failed;
      std::cerr << "Incorrectly states does not have IP type 6 port -1" << std::endl;
    } else {
      ++num_passed;
    }
    if (ip_types.isDupIP(6, -1)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port -1 expects duplicate IPs" << std::endl;
    } else {
      ++num_passed;
    }
    if (!ip_types.isDupIP(6, 161)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port 161 does not expect duplicate IPs" << std::endl;
    } else {
      ++num_passed;
    }
    if (ip_types.isDupIP(6, 2)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port 2 expects duplicate IPs" << std::endl;
    } else {
      ++num_passed;
    }
    if (ip_types.isDupMAC(6, -1)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port -1 expects duplicate MACs" << std::endl;
    } else {
      ++num_passed;
    }
    if (!ip_types.isDupMAC(6, 161)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port 161 does not expect duplicate MACs" << std::endl;
    } else {
      ++num_passed;
    }
    if (ip_types.isDupMAC(6, 2)) {
      ++num_failed;
      std::cerr << "Incorrectly states IP type 6 port 2 expects duplicate MACs" << std::endl;
    } else {
      ++num_passed;
    }
  } else {
    ++num_failed;
    std::cout << "IP types file does not exist: " << new_ip_fname << std::endl;
  }

  if(stat(eth_fname.c_str(), &filestat) == 0) {

    EthernetTypes ethernet_types;
    ethernet_types.read(eth_fname);
    if (!ethernet_types.hasType(0x0806)) {
      ++num_failed;
      std::cerr << "Incorrectly states does not have Ethernet Type 0x0806" << std::endl;
    } else {
      ++num_passed;
    }
    if (ethernet_types.hasType(0x0FFFFF)) {
      ++num_failed;
      std::cerr << "Incorrectly states has Ethernet Type 0x0FFFFF" << std::endl;
    } else {
      ++num_passed;
    }
    if (ethernet_types.isDupIP(0x0805)) {
      ++num_failed;
      std::cerr << "Incorrectly states Ethernet Type 0x0805 expects duplicate IPs" << std::endl;
    } else {
      ++num_passed;
    }
    if (!ethernet_types.isDupIP(0x0806)) {
      ++num_failed;
      std::cerr << "Incorrectly states Ethernet Type 0x0806 does not expect duplicate IPs" << std::endl;
    } else {
      ++num_passed;
    }
    if (ethernet_types.isDupMAC(0x0805)) {
      ++num_failed;
      std::cerr << "Incorrectly states Ethernet Type 0x0805 expects duplicate MACs" << std::endl;
    } else {
      ++num_passed;
    }
    if (!ethernet_types.isDupMAC(0x0806)) {
      ++num_failed;
      std::cerr << "Incorrectly states Ethernet Type 0x0806 does not expect duplicate MACs" << std::endl;
    } else {
      ++num_passed;
    }
  } else {
    ++num_failed;
    std::cout << "Ethernet type file does not exist: " << eth_fname << std::endl;
  }
  print_results(num_passed, num_failed, "for Ethernet and IP types");
  return num_failed;
}

int test_bin_compare_specific() {
  int num_passed = 0;
  int num_failed = 0;

  uint8_t ui1[] = {1, 0, 0, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui2[] = {1, 1, 0, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui3[] = {1, 1, 1, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui4[] = {2, 1, 1, 8, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui5[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui6[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 4, 0};
  uint8_t ui7[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 2, 120};
  uint8_t* uip[] = {ui1, ui2, ui3, ui4, ui5, ui6, ui7};

  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
      if (is_bin4_equal(uip[i], uip[j])) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin4_less(uip[i], uip[j])) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  for (int i=0; i<5; ++i) {
    for (int j=0; j<5; ++j) {
      if (is_bin6_equal(uip[i], uip[j])) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin6_less(uip[i], uip[j])) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  for (int i=0; i<7; ++i) {
    for (int j=0; j<7; ++j) {
      if (is_bin16_equal(uip[i], uip[j])) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin16_less(uip[i], uip[j])) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  print_results(num_passed, num_failed, "for comparing bin data with specific length.");
  return num_failed;
}

int test_bin_compare_generic() {

  int num_passed = 0;
  int num_failed = 0;

  uint8_t ui1[] = {1, 0, 0, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui2[] = {1, 1, 0, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui3[] = {1, 1, 1, 0, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui4[] = {2, 1, 1, 8, 0, 1, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui5[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 2, 0};
  uint8_t ui6[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 4, 0};
  uint8_t ui7[] = {2, 1, 1, 8, 0, 7, 0, 3, 6, 12, 9, 12, 0, 3, 2, 120};
  uint8_t* uip[] = {ui1, ui2, ui3, ui4, ui5, ui6, ui7};

  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
      if (is_bin_equal(uip[i], uip[j], 4)) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin_less(uip[i], uip[j], 4)) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  for (int i=0; i<5; ++i) {
    for (int j=0; j<5; ++j) {
      if (is_bin_equal(uip[i], uip[j], 6)) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin_less(uip[i], uip[j], 6)) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  for (int i=0; i<7; ++i) {
    for (int j=0; j<7; ++j) {
      if (is_bin_equal(uip[i], uip[j], 16)) {
        if (i == j) ++num_passed;
        else ++num_failed;
      } else {
        if (i != j) ++num_passed;
        else ++num_failed;
      }
      if (is_bin_less(uip[i], uip[j], 16)) {
        if (i < j) ++num_passed;
        else ++num_failed;
      } else {
        if (i >= j) ++num_passed;
        else ++num_failed;
      }
    }
  }
  print_results(num_passed, num_failed, "for comparing bin data with generic length.");
  return num_failed;
}

int test_file_extension() {
  int num_passed = 0;
  int num_failed = 0;
  std::string paths[] = {"/foo/bar.txt", "/foo/bar.", "/foo/bar", "/foo/bar.txt/bar.cc", 
      "/foo/bar.txt/bar.", "/foo/bar.txt/bar", "/foo/.", "/foo/..", "/foo/.hidden", 
      "/foo/..bar", ".", "..", "/", "/foo/", "nameonly"};
  std::string exts[] = {".txt", ".", "", ".cc", ".", "", "", "", "", ".bar", "", "", "", "", ""};
  for (size_t i=0; i<sizeof(paths) / sizeof(*paths); ++i) {
    std::string ext = getFileExtension(paths[i]);
    if (ext.compare(exts[i]) == 0) {
      ++num_passed;
    } else {
      ++num_failed;
      std::cerr << "getFileExtension(" << paths[i] << ")=(" << ext << ") should be (" << exts[i] << ")" << std::endl;
    }
  }
  print_results(num_passed, num_failed, "for getFileExtension.");
  return num_failed;
}

int test_filename() {
  int num_passed = 0;
  int num_failed = 0;
  std::string paths[] = {"/foo/bar.txt", "/foo/bar.", "/foo/.bar", "/foo/bar", "/foo/bar/",
      "/foo/.", "/foo/..", ".", "..", "/", "//host", "nameonly"};
  std::string exts[] = {"bar.txt", "bar.", ".bar", "bar", "", ".", "..", ".", "..", "", "host", "nameonly"};
  for (size_t i=0; i<sizeof(paths) / sizeof(*paths); ++i) {
    std::string s = getFilename(paths[i]);
    if (s.compare(exts[i]) == 0) {
      ++num_passed;
    } else {
      ++num_failed;
      std::cerr << "getFilename(" << paths[i] << ")=(" << s << ") should be (" << exts[i] << ")" << std::endl;
    }
  }
  print_results(num_passed, num_failed, "for getFilename.");
  return num_failed;
}

/**
 * This test searches for files that are specifically on a specific developer's 
 * computer. Unfortunately, the methods do not recognize paths such as
 * ~/.bashrc
 * 
 * **/
int test_file_dir_exists() {
  int num_passed = 0;
  int num_failed = 0;
  int i=0;

  ++i;

  // These things fail when you specify your home directory as
  // ~/ so don't do that. 
  std::string home=getUserHomeDir();
  std::string bashrc = home + "/.bashrc";
  std::string tmp_dir = "/tmp";

  if (!isPathExist(bashrc, false, false, false, false)) {
    ++num_failed;
    std::cout << "Failed to identify a file that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(bashrc, false, false, true, false)) {
    ++num_failed;
    std::cout << "Failed to identify a readable file that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(bashrc, false, false, false, true)) {
    ++num_failed;
    std::cout << "Failed to identify a writable file that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(bashrc, false, false, true, true)) {
    ++num_failed;
    std::cout << "Failed to identify a readable / writable file (1) that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(bashrc, true, false, true, true)) {
    ++num_failed;
    std::cout << "Failed to identify a readable / writable file (2) that exists." << std::endl;
  }
  ++i;
  if (isPathExist(bashrc, false, true, true, true)) {
    ++num_failed;
    std::cout << "This is a file, not a readable / writable directory (2) that exists." << std::endl;
  }
  ++i;
  if (isPathExist(bashrc, true, true, true, true)) {
    ++num_failed;
    std::cout << "File Cannot be both a file and directory." << std::endl;
  }
  ++i;
  if (isPathExist(tmp_dir, true, true, true, true)) {
    ++num_failed;
    std::cout << "Directory cannot be both a file and directory." << std::endl;
  }
  ++i;
  if (!isPathExist(tmp_dir, false, false, false, false)) {
    ++num_failed;
    std::cout << "Failed to identify a directory that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(tmp_dir, false, false, true, false)) {
    ++num_failed;
    std::cout << "Failed to identify a readable directory that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(tmp_dir, false, false, false, true)) {
    ++num_failed;
    std::cout << "Failed to identify a writable directory that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(tmp_dir, false, false, true, true)) {
    ++num_failed;
    std::cout << "Failed to identify a readable / writable directory (1) that exists." << std::endl;
  }
  ++i;
  if (isPathExist(tmp_dir, true, false, true, true)) {
    ++num_failed;
    std::cout << "This is a directory, not a file a readable / writable file (2) that exists." << std::endl;
  }
  ++i;
  if (!isPathExist(tmp_dir, false, true, true, true)) {
    ++num_failed;
    std::cout << "This is a directory, not a file a readable / writable file (3) that exists." << std::endl;
  }
  ++i;
  if (isPathExist(tmp_dir, true, true, true, true)) {
    ++num_failed;
    std::cout << "File Cannot be both a file and directory." << std::endl;
  }
  ++i;
  if (!isPathExist("/", false, false, false, false)) {
    ++num_failed;
    std::cout << "Failed to find /" << std::endl;
  }
  ++i;
  if (!isPathExist("/", false, true, false, false)) {
    ++num_failed;
    std::cout << "Failed to find / as a directory" << std::endl;
  }
  ++i;
  if (!isPathExist("/", false, true, true, false)) {
    ++num_failed;
    std::cout << "Failed to find / as a readable directory" << std::endl;
  }
  ++i;
  if (isPathExist("/", false, true, false, true)) {
    ++num_failed;
    std::cout << "Found / as a writeable directory" << std::endl;
  }
  ++i;
  if (isPathExist("/", false, true, true, true)) {
    ++num_failed;
    std::cout << "Found / as a readable / writeable directory" << std::endl;
  }
  num_passed = i - num_failed;

  print_results(num_passed, num_failed, "Check file and directory permissions.");
  return num_failed;
}

int test_replace_all() {
  int num_failed = 0;
  int num_passed = 0;
  std::string strings[] = {
    "", "", "", "",
    "", "a", "", "",
    "a", "a", "", "",
    "a", "a", "a", "a",
    "aaa", "a", "aa", "aaaaaa",
    "a\"b", "\"", "\"\"", "a\"\"b",
  };
  int num_strings = sizeof(strings) / sizeof(*strings);
  std::cout << "There are " << num_strings << " strings." << std::endl;
  for (int i=0; i<num_strings; i += 4) {
    std::string s = replaceAll(strings[i], strings[i+1], strings[i+2]);
    if (s == strings[i+3]) {
      ++num_passed;
    } else {
      ++num_failed;
      std::cout << "replaceAll(\"" << strings[i] << "\", \"" << strings[i+1] << "\", \"" << strings[i+2] << "\") != \"" << s << "\" but expected \"" << strings[i+3] << "\"" << std::endl;;
    }
  }

  std::cout << std::endl << "replaceAll passed " << num_passed << " tests." << std::endl;
  if (num_failed > 0)
    std::cout << std::endl << "replaceAll failed " << num_failed << " tests." << std::endl;
  return num_failed;
}


int main(int , char **) {
  std::cout << std::endl;
  int num_failed = 0;
  num_failed += test_aho_overlap();
  num_failed += test_search();
  num_failed += test_bits();
  num_failed += test_ip_mac();
  num_failed += test_bin_compare_specific();
  num_failed += test_bin_compare_generic();
  num_failed += test_filename();
  num_failed += test_file_extension();
  num_failed += test_file_dir_exists();
  num_failed += test_replace_all();

  struct ip ipHeader;
  ipHeader.ip_p = 6;
  std::cout << std::endl;
  if (ipHeader.ip_p == IPPROTO_TCP) {
    std::cout << "PASS: Set ipHeader.ip_p to 6 and compare to IPPROTO_TCP" << std::endl;
  } else {
    std::cout << "FAIL: Set ipHeader.ip_p to 6 and compare to IPPROTO_TCP" << std::endl;
    ++num_failed;
  }

  std::cout << std::endl;
  if (num_failed > 0)
    std::cout << "ERROR *********: number of tests failed:" << num_failed << std::endl;

  return -num_failed;
}
