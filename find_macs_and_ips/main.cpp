#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <iomanip>
#include <cstring>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "pcap.h"

// Evil global variable to hold an ethernet type description
std::unordered_map<unsigned int, std::string> eth_types_description;

void mac_to_int(const u_int8_t *mac, u_int64_t *out){
  /*Pack a MAC address (array of 6 unsigned chars) into an integer for easy inclusion in a set*/
  *out = (u_int64_t)(mac[0])<<40;
  *out += (u_int64_t)(mac[1])<<32; 
  *out += (u_int64_t)(mac[2])<<24;
  *out += (u_int64_t)(mac[3])<<16;
  *out += (u_int64_t)(mac[4])<<8;
  *out += (u_int64_t)(mac)[5];
}

void int_to_mac(u_int64_t mac, u_int8_t *out){
  //Populate an array of 6 8-bit integergs from a 48-bit integer that has been packed in a 64-bit integer
  //This is intended to turn an integer into a more human-usable MAC address array
  //   of the kind used by Ethernet structs from PCAP data
  out[0] = mac>>40 & 0xff;
  out[1] = mac>>32 & 0xff;
  out[2] = mac>>24 & 0xff;
  out[3] = mac>>16 & 0xff;
  out[4] = mac>>8 & 0xff;
  out[5] = mac & 0xff;
}

void print_mac(const u_int8_t *mac){
  // Print an array of 6 8-bit integers to std::cout (which might be a file) formatted as normal MAC addresses
  std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[0]) << ":"
	    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[1]) << ":"
	    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[2]) << ":"
	    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[3]) << ":"
	    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[4]) << ":"
	    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[5]) << std::endl;

}

void usage(){
  printf("Usage:\n");
  printf("-p <path to IP output filename, default 'ip_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all IP addresses found in the input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-m <path to MAC output filename, default 'mac_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all Ethernet MAC addresses input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses\n");
  printf("-c create the list of contained IP and MAC addresses.\n");
  printf("\n");
}

int write_ip_and_mac_from_pcap(const char*pcap_fname, const char *out_mac_fname, const char *out_ip_fname) {
  pcap_t *pcap_file;
  char *pcap_errbuf = nullptr;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  const u_int8_t *shost, *dhost;
  u_int done=0;
  int res=1, it=0;
  std::ofstream mac_out;
  std::ofstream ip_out;

  std::set<u_int64_t> unique_macs;
  std::set<std::string> unique_ips;
  u_int64_t mac;
  u_int8_t eth_mac[ETH_ALEN];
  char ip_addr_max[INET6_ADDRSTRLEN]={0};
  std::streambuf *coutbuf = std::cout.rdbuf(); // Save the coutbuf in case we later direct to a file

  pcap_file=pcap_open_offline(pcap_fname, pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_fname);
  }

  // Iterate over every packet in the file and print the MAC addresses
  while(!done){
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);
  
    if(res == PCAP_ERROR_BREAK){
      fprintf(stderr, "No more packets in savefile. Iteration %d\n", it);
      break;
    }
    if(res != 1){
      fprintf(stderr, "Error reading packet. Iteration %d\n", it);
      continue;
    }
    ether = (const struct ether_header*)pkt_data;

    // Extract the frame MACs and put them into the set for uniqueness discovery
    shost = ether->ether_shost;
    dhost = ether->ether_dhost;

    mac_to_int(shost, &mac);
    unique_macs.insert(mac);

    mac_to_int(dhost, &mac);
    unique_macs.insert(mac);

    if ((ntohs(ether->ether_type) != ETHERTYPE_IP) && (ntohs(ether->ether_type) != ETHERTYPE_IPV6)){
      // If the frame doesn't contain IP headers, skip extracting the IPs. Cause they don't exist.
      continue;
      it++;
    }
    
    // If we make it here, there should be IPs in the frame.
    if (ntohs(ether->ether_type) == ETHERTYPE_IP) {
      const struct ip* ipHeader;
      ipHeader = (struct ip*)(pkt_data + sizeof(struct ether_header));
      // Turn the raw src and dst IPs in the packet into human-readable
      //   IPs and insert them into the set
      // Be sure to clear the char* buffer each time so that the end
      //   of the IP will always be correctly null-terminated
      // If we don't do this, we risk keeping junk from the previous IP
      //   that may have been longer than the current IP.
      inet_ntop(AF_INET, &(ipHeader->ip_src), ip_addr_max, INET_ADDRSTRLEN);
      unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip_dst), ip_addr_max, INET_ADDRSTRLEN);
      unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      inet_ntop(AF_INET, &(ipHeader->ip6_src), ip_addr_max, INET6_ADDRSTRLEN);
      unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip6_dst), ip_addr_max, INET6_ADDRSTRLEN);
      unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }

    it++;
  }

  pcap_close(pcap_file);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

  // Write the unique MACs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_mac_fname != nullptr){
    // If the user gave us an output 
    mac_out.open(out_mac_fname);
    std::cout.rdbuf(mac_out.rdbuf());
  }
  std::for_each(unique_macs.begin(), unique_macs.end(), [&eth_mac, &mac_out](u_int64_t m){
      int_to_mac(m, eth_mac);
      print_mac(eth_mac);
    });
  std::cout.rdbuf(coutbuf);


  // Write the unique IPs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_ip_fname != nullptr){
    // If the user gave us an output 
    ip_out.open(out_ip_fname);
    std::cout.rdbuf(ip_out.rdbuf());
  }
  std::for_each(unique_ips.begin(), unique_ips.end(), [&ip_out](std::string ip){
      std::cout << ip << std::endl;
    });
  std::cout.rdbuf(coutbuf);

  return 0;
}

// Written to avoid compiler warnings.
bool startsWith(const char* s, char c) {
  return (s != nullptr && s[0] == c);
}

int main(int argc, char **argv){

  /*Given an input PCAP file, discover all unique MAC addresses and IPs and write them to a file (stdout by default
   */
  char *pcap_fname=0;
  // We need two distinct output files for MACs and IPs for some compatibility with the existing script
  char _mac_fname[]="mac_addresses.txt";
  char _ip_fname[]="ip_addresses.txt";
  char *out_mac_fname=_mac_fname;
  char *out_ip_fname=_ip_fname;
  int index, arg;

  struct stat filestat;

  bool create_ip_and_mac = false;
  while((arg = getopt(argc, argv, "hm:p:r:c")) != -1){
    switch(arg) {
    case 'p':
      if (startsWith(optarg, '-')){out_ip_fname=nullptr;}
      else {out_ip_fname=optarg;}
      break;
    case 'm':
      if (startsWith(optarg, '-')){out_mac_fname=nullptr;}
      else {out_mac_fname=optarg;}
      break;
    case 'c':
      create_ip_and_mac=true;
      break;
    case 'r':
      pcap_fname=optarg;
      break;
    case 'h':
      usage();
      exit(0);
    case '?':
      if (optopt == 'o'){ std::cerr << "Option -" << optopt <<" requires a filename argument for output" << std::endl; }
      else if (optopt == 'r'){ std::cerr << "Option -" << optopt <<" requires a filename argument for output" << std::endl; }
      else {
	std::cerr << "Unknown option -"<< optopt << std::endl;
      } 
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

  if (create_ip_and_mac)
    write_ip_and_mac_from_pcap(pcap_fname, out_mac_fname, out_ip_fname);

  return 0;
}
