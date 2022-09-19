#include <algorithm>
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pcap.h"
#include "macaddresses.h"
#include "ipaddresses.h"

void print_mac(const uint8_t *mac){
  // Print an array of 6 8-bit integers to std::cout (which might be a file) formatted as normal MAC addresses
  std::cout << MacAddresses::mac_to_str(mac) << std::endl;
}

void usage(){
  printf("Usage:\n");
  printf("-p <path to IP output filename, default 'ip_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all IP addresses found in the input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-m <path to MAC output filename, default 'mac_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all Ethernet MAC addresses input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses\n");
  printf("\n");
}

bool startsWith(const char* s, char c) {
  return (s != nullptr && s[0] == c);
}

int main(int argc, char **argv){
  /*Given an input PCAP file, discover all unique MAC addresses and IPs and write them to a file (stdout by default
   */
  char *in_fname=0;
  // We need two distinct output files for MACs and IPs for some compatibility with the existing script
  char _mac_fname[]="mac_addresses.txt";
  char _ip_fname[]="ip_addresses.txt";
  char *out_mac_fname=_mac_fname;
  char *out_ip_fname=_ip_fname;
  u_int done=0;
  int res=1, it=0, index, arg;

  struct stat filestat;
  std::ofstream mac_out;
  std::ofstream ip_out;
  std::streambuf *coutbuf = std::cout.rdbuf(); // Save the coutbuf in case we later direct to a file
  pcap_t *pcap_file;
  char *pcap_errbuf = nullptr;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  const uint8_t *shost, *dhost;

  std::set<u_int64_t> unique_macs;
  std::set<std::string> unique_ips;
  u_int64_t mac;
  uint8_t eth_mac[ETH_ALEN];
  char ip_addr_max[INET6_ADDRSTRLEN]={0};

  while((arg = getopt(argc, argv, "hm:p:r:")) != -1){
    switch(arg) {
    case 'p':
      if (startsWith(optarg, '-')){out_ip_fname=0;}
      else {out_ip_fname=optarg;}
      break;
    case 'm':
      if (startsWith(optarg, '-')){out_mac_fname=0;}
      else {out_mac_fname=optarg;}
      break;
    case 'r':
      in_fname=optarg;
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
  if(in_fname == NULL){
    usage();
    exit(1);
  }

  if(stat(in_fname, &filestat) != 0){
    std::cerr << "Input file "<<in_fname<<" is not accessible" <<std::endl;
    exit(1);
  }
  
  pcap_file=pcap_open_offline(in_fname, pcap_errbuf);

  /*Ensure that the pcap file only has Ethernet packets*/
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", in_fname);
  }

  /*Iterate over every packet in the file and print the MAC addresses*/
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

    MacAddresses::mac_to_int(shost, &mac);
    unique_macs.insert(mac);

    MacAddresses::mac_to_int(dhost, &mac);
    unique_macs.insert(mac);

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
  if(out_mac_fname != NULL){
    // If the user gave us an output 
    mac_out.open(out_mac_fname);
    std::cout.rdbuf(mac_out.rdbuf());
  }
  std::for_each(unique_macs.begin(), unique_macs.end(), [&eth_mac, &mac_out](u_int64_t m){
      MacAddresses::int_to_mac(m, eth_mac);
      print_mac(eth_mac);
    });
  std::cout.rdbuf(coutbuf);


  // Write the unique IPs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_ip_fname != NULL){
    // If the user gave us an output 
    ip_out.open(out_ip_fname);
    std::cout.rdbuf(ip_out.rdbuf());
  }
  std::for_each(unique_ips.begin(), unique_ips.end(), [&ip_out](std::string ip){
      std::cout << ip << std::endl;
    });
  std::cout.rdbuf(coutbuf);

  // Note that I can do things like this:
  //MacAddresses mac_addresses;
  //mac_addresses.read_file(out_mac_fname);
  //mac_addresses.write_file("temp_mac.txt");

  //IpAddresses ip_addresses;
  //ip_addresses.read_file(out_ip_fname);
  //ip_addresses.write_file("temp_ip.txt");

  return 0;
}
