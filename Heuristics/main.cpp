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
#include "utilities.h"
#include "iptype.h"
#include "ethtype.h"
#include "crc32_x.h"

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
  printf("-h Print this help.\n");
  printf("-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses\n");
  printf("-a <path to generated anomaly pcap>: Where to write the anomaly list. This triggers the creation of the anomoly list.\n");
  printf("\n");
}

//
// Read a text file one line at a time and add it to an unordered set. 
// This can read MAC addresses as well as IP addresses. 
//
std::unordered_set<std::string>* read_text_file(const char* fname) {
  if (fname == nullptr) {
    printf("Ethernet type file name is null\n\n");
    usage();
    return nullptr;
  }

  //
  // Probably not safe to assume that C++17 is availble so do not use <filesystem> such as
  // std::filesystem::path f{"file.txt"};
  // if (std::filesystem::exists(f)) ...
  //
  std::ifstream file(fname);
  if(!file.is_open()){
    std::cout << "File not found" << std::endl;
    return nullptr;
  }
  std::unordered_set<std::string>* lines = new std::unordered_set<std::string>();
  std::string line;
  // Note that reading a file using a FILE* is 4 to 5 times faster.
  // Processing speed is not an issue since this is not done often and the file is small.
  while (std::getline(file, line)) {
    //
    // remove leading and trailing spaces from the string.
    //
    trim(line);
    //
    // Ignore blank lines and lines begining with the '#' character. 
    //
    if (line.length() == 0 || line.front() == '#') {
      continue;
    }
    if (lines->find(line) == lines->end())
      lines->insert(line);
  }
  return lines;
}

inline bool is_valid_ether_type(const struct ether_header *ether, const std::unordered_set<unsigned int>& eth_types) {
  return (ether != nullptr && eth_types.find(ntohs(ether->ether_type)) != eth_types.end());
}

//**************************************************************************
//! Read the pcap file and create the anomaly file based on the Heuristic
/*!
 * 
 * \param [in] ethernet_types List of valid Ethernet (types) and which allow for a repeated IP or MAC in the payload.
 * 
 * \param [in] ip_types List of valid IP protocols (types) and which allow for a repeated IP or MAC in the payload.
 * 
 * \param [in] eth_types List of valid Ethernet types
 * 
 * \param [in] pcap_fname Full path to the input PCAP file
 * 
 * \param [in] anomaly_fname Full path to the generated PCAP file with anomalies.
 * 
 * \returns 0 on no error, not very useful at this time.
 *
 ***************************************************************************/
int create_heuristic_anomaly_file(const EthernetTypes& ethernet_types, const IPTypes& ip_types, const char* pcap_fname, const char* anomaly_fname) {
  pcap_t *pcap_file;
  pcap_dumper_t *dumpfile;
  bool done = false;


  char *pcap_errbuf;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  const u_int8_t *shost, *dhost;
  int res=1, it=0, i=0, index;
  u_int64_t mac;
  u_int8_t eth_mac[ETH_ALEN];
  char ip_addr_max[INET6_ADDRSTRLEN]={0};


  // Open the PCAP file!
  pcap_file=pcap_open_offline(pcap_fname, pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_fname);
    pcap_close(pcap_file);
    return -1;
  }

  // Open the dump file
  dumpfile = pcap_dump_open(pcap_file, anomaly_fname);

  if(dumpfile==NULL)
  {
    pcap_close(pcap_file);
    fprintf(stderr,"\nError opening output file\n");
    return -1;
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

    // ??
    // Do I need to do any of this? 
    mac_to_int(shost, &mac);
    //unique_macs.insert(mac);
    mac_to_int(dhost, &mac);
    //unique_macs.insert(mac);


    //?? Originally filtered based on this, which allows only IPv4 (0x0800) and IPv6 (0x8gdd)
    /**
    if (ntohs(ether->ether_type) != ETHERTYPE_IP & ntohs(ether->ether_type) != ETHERTYPE_IPV6){
      // If the frame doesn't contain IP headers, skip extracting the IPs. Cause they don't exist.
      it++;
      continue;
    }
    **/

    if (!ethernet_types.isValid(ntohs(ether->ether_type))) {
      it++;
      std::cout << "Found invalid ether type " << ntohs(ether->ether_type) << std::endl;
      // TODO: CHeck for valid FCS
      // I do not like that we cast dumpfile to (uchar *)
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      continue;
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
      //unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip_dst), ip_addr_max, INET_ADDRSTRLEN);
      //unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      inet_ntop(AF_INET, &(ipHeader->ip6_src), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip6_dst), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }

    it++;
  }

  //?? pcap_dump(dumpfile, header, pkt_data);
  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

  return 0;
}



//
// Read the pcap file and create the anomaly file
//
// TODO:
// This is NOT used. Remove this. 
// Only leaving right now so that I can see example code it contains. 
int create_anomaly_file(const EthernetTypes& ethernet_types, const std::unordered_set<std::string>& ips, const std::unordered_set<std::string>& macs, const char* pcap_fname, const char* anomaly_fname) {
  pcap_t *pcap_file;
  char *pcap_errbuf;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  const u_int8_t *shost, *dhost;
  u_int done=0;
  int res=1, it=0, i=0, index;
  u_int64_t mac;
  u_int8_t eth_mac[ETH_ALEN];
  char ip_addr_max[INET6_ADDRSTRLEN]={0};


  // map is ordered and has O(log) access time. 
  // unordered_map is a hashmap with O(1) access time.
  // This was introduced with C++11 so is mostly supported by now.
  // #include <tr1/unordered_map>
  // std::tr1::unordered_map
  std::unordered_map<unsigned int, unsigned int> eth_types_count;

  // xyzzy
  // Save the coutbuf in case we later direct to a file
  std::streambuf *coutbuf = std::cout.rdbuf();
  std::ofstream anomaly_out;
  if(anomaly_fname != nullptr){
    anomaly_out.open(anomaly_fname);
    if (anomaly_out.good())
      std::cout.rdbuf(anomaly_out.rdbuf());
  }


  pcap_file=pcap_open_offline(pcap_fname, pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_fname);
    // TODO: Should we not just exit here? 
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

    // ??
    // Do I need to do any of this? 
    mac_to_int(shost, &mac);
    //unique_macs.insert(mac);
    mac_to_int(dhost, &mac);
    //unique_macs.insert(mac);


    //?? Originally filtered based on this, which allows only IPv4 (0x0800) and IPv6 (0x8gdd)
    /**
    if (ntohs(ether->ether_type) != ETHERTYPE_IP & ntohs(ether->ether_type) != ETHERTYPE_IPV6){
      // If the frame doesn't contain IP headers, skip extracting the IPs. Cause they don't exist.
      it++;
      continue;
    }
    **/

    if (eth_types_count.find(ntohs(ether->ether_type)) == eth_types_count.end()) {
      eth_types_count[ntohs(ether->ether_type)] = 1;
    } else {
      ++eth_types_count[ntohs(ether->ether_type)];
    }

    if (!ethernet_types.isValid(ntohs(ether->ether_type))) {
      it++;
      continue;
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
      //unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip_dst), ip_addr_max, INET_ADDRSTRLEN);
      //unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      inet_ntop(AF_INET, &(ipHeader->ip6_src), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      inet_ntop(AF_INET, &(ipHeader->ip6_dst), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
    }

    it++;
  }

  pcap_close(pcap_file);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

//?? remove this section
  /**
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
**/

  // Restore the cout buffer
  std::cout.rdbuf(coutbuf);

  return 0;
}

int write_ip_and_mac_from_pcap(const char*pcap_fname, const char *out_mac_fname, const char *out_ip_fname) {
  pcap_t *pcap_file;
  char *pcap_errbuf;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  const u_int8_t *shost, *dhost;
  u_int done=0;
  int res=1, it=0, i=0, index;
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

    if (ntohs(ether->ether_type) != ETHERTYPE_IP & ntohs(ether->ether_type) != ETHERTYPE_IPV6){
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



int main(int argc, char **argv){

  // TODO: Move this as appropriate. 
  //
  // This is how to read the ip_types.txt file, which will 
  // then be sent to the processor for the pcap file.
  // Still need to edit ip_types.txt so that it will have 
  // correct values for types that support repeating IP or MAC addresses.
  // See the spreadsheet uploaded by Beau.
  //

  std::string ip_fname = "ip_types.txt";
  IPTypes ip_types;
  ip_types.read(ip_fname);
  //std::cout << ip_types;
  //std::cout << std::endl;


  std::string eth_fname = "eth_types.txt";
  EthernetTypes ethernet_types;
  ethernet_types.read(eth_fname);
  //std::cout << ethernet_types;
  //std::cout << std::endl;

  /*Given an input PCAP file, discover all unique MAC addresses and IPs and write them to a file (stdout by default
   */
  char *pcap_fname=0;
  int index, arg;

  struct stat filestat;

  bool create_anomaly_list = false;
  const char* anomaly_fname = nullptr;

  while((arg = getopt(argc, argv, "hr:a:")) != -1){
    switch(arg) {
    case 'a':
      create_anomaly_list=true;
      if(optarg == "-"){anomaly_fname=nullptr;}
      else {anomaly_fname=optarg;}
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

  if (create_anomaly_list) {
    std::cout << "Ready to go " << std::endl;
    create_heuristic_anomaly_file(ethernet_types, ip_types, pcap_fname, anomaly_fname);
  }

  return 0;
}
