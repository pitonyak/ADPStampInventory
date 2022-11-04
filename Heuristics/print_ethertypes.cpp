#include <cstdint>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <locale>
#include <net/ethernet.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ethtype.h"
#include "pcap.h"
#include "utilities.h"

bool verbose = false;

void usage(){
  std::cout << "Usage:" << std::endl;
  std::cout << "-h Print this help." << std::endl;
  std::cout << "-r <path to input pcap file>: This PCAP file will be read to find ethernet types." << std::endl;
  std::cout << "-l Lower bound, first packet to print, defaults to 0." << std::endl;
  std::cout << "-u Upper bound, last packet to evaluate, defaults to -1 so print everything after lower bound." << std::endl;
  std::cout << "-v Print a hex dump of the ethernet header, last two bytes are the ethernet type." << std::endl;
  std::cout << std::endl;
  std::cout << "Sample output in verbose mode:" << std::endl;
  std::cout << "0 type:8 ntohs(type):800 swap:800 no swap:8 isValid:1  hex: 0 16 3e 27 77 db 98 5d 82 11 54 49 8 0" << std::endl;
  std::cout << std::endl;
  std::cout << "This is packet #0 (so the first packet)" << std::endl;
  std::cout << "The type as read is hex: 08 00 so not accounting for LSB the type shows as 0x08." << std::endl;
  std::cout << "In C++, ntohs converts network byte order to local byte order so type is 0x800." << std::endl;
  std::cout << "Swap shows the value if byte order is swapped (0x800) and no swap shows if the order is not (0x008)." << std::endl;
  std::cout << "isValid indicates if the type is considered a valid ethernet type (1) or not (0)." << std::endl;
  std::cout << "The last bytes show the ethernet header, the last two bytes are the type." << std::endl;
  std::cout << std::endl;
}


uint16_t endian_swap_uint16(const uint8_t* c) {
  return (c == nullptr) ? 0 : 256 * c[0] +  c[1];
}

uint16_t endian_no_swap_uint16(const uint8_t* c) {
  return (c == nullptr) ? 0 : 256 * c[1] +  c[0];
}


int print_ether_types(const std::string& pcap_fname, long lower_bound, long upper_bound, const EthernetTypes& ethernet_types) {

  pcap_t *pcap_file;
  char *pcap_errbuf = nullptr;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  u_int done=0;
  int res=1;
  long it = 0;

  pcap_file=pcap_open_offline(pcap_fname.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    std::cerr << "PCAP file " << pcap_fname << "is not an Ethernet capture\n" << std::endl;
  }

  // Iterate over every packet in the file and print the MAC addresses
  while(!done){
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);
  
    if(res == PCAP_ERROR_BREAK){
      std::cerr << "No more packets in savefile. Iteration " << it << std::endl;
      break;
    }
    if(res != 1){
      std::cerr << "Error reading packet. Iteration " << it << std::endl;
      continue;
    }
    if (lower_bound <= upper_bound && upper_bound < it) {
      break;
    }
    if (lower_bound > upper_bound || (lower_bound <= it && it <= upper_bound)) {
      // Print the ethertype!
      ether = (const struct ether_header*)pkt_data;
      const u_char* type_start = pkt_data + 2 * ETH_ALEN;
      uint16_t swap_et = endian_swap_uint16((const uint8_t*)type_start);
      uint16_t no_swap_et = endian_no_swap_uint16((const uint8_t*)type_start);

      // ntohs() converts the unsigned short integer netshort 
      // from network byte order to host byte order.
      // The standard network byte order is big-endian.
      // Intel x86 CPUs are little-endian.
      uint16_t ntohs_et = ntohs(ether->ether_type);
      std::cout << it << std::hex << " type:" << ether->ether_type << " ntohs(type):" << ntohs_et <<  " swap:" << swap_et << " no swap:" << no_swap_et << " isValid:" << ethernet_types.isValid(ntohs_et) << std::dec;
      if (verbose) {
        std:: cout << "  hex: ";
        dump_hex(pkt_data, 2 * ETH_ALEN + 2);
      } else {
        std::cout << std::endl;
      }
    }
    ++it;
  }

  pcap_close(pcap_file);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

  return 0;
}

bool startsWith(const char* s, char c) {
  return (s != nullptr && s[0] == c);
}

int main(int argc, char **argv){

  std::string eth_fname = "eth_types.txt";
  EthernetTypes ethernet_types;
  ethernet_types.read(eth_fname);
  //std::cout << ethernet_types;
  //std::cout << std::endl;

  /*Given an input PCAP file, discover all unique MAC addresses and IPs and write them to a file (stdout by default
   */
  const char* pcap_fname= nullptr;
  const char* lower_arg = nullptr;
  const char* upper_arg = nullptr;
  int index, arg;

  struct stat filestat;

  while((arg = getopt(argc, argv, "vhl:u:r:")) != -1){
    switch(arg) {
    case 'v':
      verbose = true;
      break;
    case 'l':
      lower_arg=optarg;
      break;
    case 'u':
      upper_arg=optarg;
      break;
    case 'r':
      pcap_fname=optarg;
      break;
    case 'h':
      usage();
      exit(0);
    case '?':
      std::cerr << "Unknown option -"<< optopt << std::endl;
    }
  }

  // If any argument was given that isn't a known option, print the usage and exit
  for (index = optind; index < argc; index++){
    usage();
    exit(1);
  }

  // -r <fname> is required as an argument
  if(pcap_fname == nullptr){
    usage();
    exit(1);
  }

  if(stat(pcap_fname, &filestat) != 0){
    std::cerr << "Input file "<<pcap_fname<<" is not accessible" <<std::endl;
    exit(1);
  }

  std::string pcap_fname_s(pcap_fname);
  if (!isPathExist(pcap_fname_s, true, false, false, false)) {
    std::cout << "PCAP file does not exist: " << pcap_fname_s << std::endl;
    return -1;
  }

  if (!isPathExist(pcap_fname_s, true, false, true, false)) {
    std::cout << "Do not have read permission on PCAP file: " << pcap_fname_s << std::endl;
    std::cout << "This may fail to read the PCAP file" << std::endl;
    //return -1;
  }

  long lower_bound = (lower_arg == nullptr) ? 0 : std::stol(lower_arg);
  long upper_bound = (upper_arg == nullptr) ? -1 : std::stol(upper_arg);
  print_ether_types(pcap_fname_s, lower_bound, upper_bound, ethernet_types);




  return 0;
}
