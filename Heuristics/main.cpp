#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <fstream>
#include <functional> 
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <locale>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "pcap.h"
#include "utilities.h"
#include "iptype.h"
#include "ethtype.h"
#include "crc32_x.h"

#include "macaddresses.h"
#include "ipaddresses.h"

// Evil global variable to hold an ethernet type description
std::unordered_map<unsigned int, std::string> eth_types_description;

// The MAC and IP addresses in this file.
MacAddresses mac_addresses;
IpAddresses ip_addresses;

void usage(){
  printf("Usage:\n");
  printf("-h Print this help.\n");
  printf("-v Print verbose output while processing the file.\n");
  printf("-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses\n");
  printf("-a <path to generated anomaly pcap>: Where to write the anomaly list. This triggers the creation of the anomoly list.\n");
  printf("-p <path to IP output filename, default '???ip_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all IP addresses found in the input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-m <path to MAC output filename, default '???mac_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all Ethernet MAC addresses input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
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

//**************************************************************************
//! Search for a repeated MAC address. 
/*!
 * 
 * \param [in] search_start_loc where to start searching
 * 
 * \param [in] search_len maximum length to search
 * 
 * \param [in, out] dumpfile where to dump the packet if something is found
 * 
 * \param [in] pkt_header needed to dump the packet
 * 
 * \param [in] pkt_data needed to dump the packet
 * 
 * \param [in] verbose if True, explanation text is written to the screen.
 * 
 * \param [in] it current iteration.
 * 
 * \returns True if a duplicate mac is found.
 *
 ***************************************************************************/
bool find_macs(const u_int8_t* search_start_loc, u_int32_t search_len, pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it) {

  for (auto const &a_mac: mac_addresses.m_unique_macs) {
    if (find_match(a_mac, 6, search_start_loc, search_len)) {
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      if (verbose) {
        std::cout << it << " MAC address found in data." << std::endl;
        dump_hex(pkt_data, pkt_header->len);
      }
      return true;
    }
  }
  return false;
}

//**************************************************************************
//! Search for a repeated IP address. 
/*!
 * 
 * \param [in] search_start_loc where to start searching
 * 
 * \param [in] search_len maximum length to search
 * 
 * \param [in, out] dumpfile where to dump the packet if something is found
 * 
 * \param [in] pkt_header needed to dump the packet
 * 
 * \param [in] pkt_data needed to dump the packet
 * 
 * \param [in] verbose if True, explanation text is written to the screen.
 * 
 * \param [in] it current iteration.
 * 
 * \returns True if a duplicate mac is found.
 *
 ***************************************************************************/
bool find_ips(const u_int8_t* search_start_loc, u_int32_t search_len, pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it) {

  for (auto const &an_ip: ip_addresses.m_unique_ipv4) {
    if (find_match(an_ip, 4, search_start_loc, search_len)) {
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      if (verbose) {
        std::cout << it << " IPv4 address found in data" << std::endl;
        dump_hex(pkt_data, pkt_header->len);
      }
      return true;
    }
  }

  for (auto const &an_ip: ip_addresses.m_unique_ipv6) {
    if (find_match(an_ip, 16, search_start_loc, search_len)) {
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      if (verbose) {
        std::cout << it << " IPv6 address found in data" << std::endl;
        dump_hex(pkt_data, pkt_header->len);
      }
      return true;
    }
  }

  return false;
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
 * \param [in] verbose Print more than usual output.
 * 
 * \returns 0 on no error, not very useful at this time.
 *
 ***************************************************************************/
int create_heuristic_anomaly_file(const EthernetTypes& ethernet_types, const IPTypes& ip_types, const char* pcap_fname, const char* anomaly_fname, bool verbose) {
  pcap_t *pcap_file;
  pcap_dumper_t *dumpfile;
  bool done = false;

  char *pcap_errbuf;
  //
  // This structure has three fields:
  // struct timeval ts (time stamp)
  // u_int32 caplen (length of portion present)
  // u_int32 len (length of this packet off wire)
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;

  // 6 byte MAC Address
  const u_int8_t *shost, *dhost;

  // 4 byte (IPv4) or 16 byte (IPv6) IP Address.
  const u_int8_t *ip_src, *ip_dst;

  int res=1, it=0, i=0;
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

  const int offset_to_data_ipv4 = sizeof(struct ether_header) + sizeof(struct ip);
  const int offset_to_data_ipv6 = sizeof(struct ether_header) + sizeof(struct ip6_hdr);

  // Iterate over every packet in the file and print the MAC addresses
  while(!done){
    // pkt_header contains three fields:
    // struct timeval ts (time stamp) with tv_sec and tv_usec for seconds and micro-seconds I guess.
    // u_int32 caplen (length of portion present)
    // u_int32 len (length of this packet off wire)
    //
    // The length fields are probably the same (from what I have seen from a very small sample set)
    // The time is used as follows:
    // #include <ctime>
    // time_t ttime = pkt_header->ts.tv_sec;
    // tm *local_time = localtime(&ttime);
    // std::cout << " Time: "<< 1 + local_time->tm_hour << ":";
    // std::cout << 1 + local_time->tm_min << ":";
    // std::cout << 1 + local_time->tm_sec << "." << pkt_header->ts.tv_usec << " " << 1 + local_time->tm_mon << "/" << local_time->tm_mday << "/" << 1900 + local_time->tm_year << std::endl;
    //
    // pkt_data points to the data.
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);

    //void * p1 = (void *) pkt_header;
    //void * p2 = (void *) pkt_data;
    //std::cout << " p1 = " << p1 << " p2 = " << p2 << std::endl;

    // What is the address of the packet header and data? Do they start at the same place? 
    // Seems silly if they do. 
  
    if(res == PCAP_ERROR_BREAK){
      fprintf(stderr, "No more packets in savefile. Iteration %d\n", it);
      break;
    }
    if(res != 1){
      fprintf(stderr, "Error reading packet. Iteration %d\n", it);
      continue;
    }

    // The only purpose for this code is to show what is in the packet header
    if (verbose && it < 10) {
      time_t ttime = pkt_header->ts.tv_sec;
      tm *local_time = localtime(&ttime);
      std::cout << "index " << it << " caplen:" << pkt_header->caplen << " len:" << pkt_header->len << " ts:" << pkt_header->ts.tv_sec << "." << pkt_header->ts.tv_usec;
      std::cout << " Time: "<< 1 + local_time->tm_hour << ":";
      std::cout << 1 + local_time->tm_min << ":";
      std::cout << 1 + local_time->tm_sec << "." << pkt_header->ts.tv_usec << " " << 1 + local_time->tm_mon << "/" << local_time->tm_mday << "/" << 1900 + local_time->tm_year << std::endl;
    }

    // Make sure that the packet data is long enough to have an ethernet header of length 12.
    if (pkt_header->len < 12) {
      if (verbose)
        std::cout << it << " packet length is too small (" << pkt_header->len << ")" << std::endl;

      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;
    }

    // The packet data begins with the Ethernet header, so if we cast to that:
    // uint8_t   ether_dhost [6]  MAC address
    // uint8_t   ether_shost [6]  MAC address
    // uint16_t  etherType        Ethernet type
    ether = (const struct ether_header*)pkt_data;

    // IPv4 address is 4 bytes
    // IPv6 address is 16 bytes
    // MAC address is 6 bytes

    // Extract the frame MACs and put them into the set for uniqueness discovery
    shost = ether->ether_shost;
    dhost = ether->ether_dhost;
    int ether_type_int = ntohs(ether->ether_type);

    // Check for type IP (0x800)
    // This type is skipped for some types, but not others.
    // Set the eth_types.txt file so that it does NOT indicate that
    // this type has copies of the IP or MAC and rely on this check instead.
    // The header follows the ether_header
    if (ether_type_int == ETHERTYPE_IP) {
      if (pkt_header->len < offset_to_data_ipv4) {
        if (verbose)
          std::cout << it << " packet length is too small to contain an IPv4 header (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }

      const struct ip* ipHeader;
      ipHeader = (struct ip*)(pkt_data + sizeof(struct ether_header));

      // So, what does an IPv4 header look like? 
      //  4-bits = [ip_v] Version, so 0100 for IPv4 (byte 0)
      //  4-bits = [ip_hl] HELEN (header length)
      //  8-bits = [ip_tos] Service Type            (byte 1)
      // 16-bits = [ip_len] total Length            (byte 2)
      // 16-bits = [ip_id] Identification           (byte 4)
      //  3-bits = Flags                            (byte 6)
      // 13-bits = [lp_off] Fragmentation offset
      //  8-bits = [ip_ttl] Time to live            (byte 8)
      //  8-bits = [ip_p] Protocol                  (byte 9) (17=UDP, 6=TCP) source / destination port.
      // 16-bits = [ip_sum] Header Checksum         (byte 10)
      // 32-bits = [ip_src] Source IP Address       (byte 12)
      // 32-bits = [ip_dst] Destination IP Address  (byte 16)
      // Rest of the Data                           (byte 20)

      int tcp_destination_port = -1;
      if (ipHeader->ip_p == IPPROTO_TCP) {

        // IPPROTO_TCP = 6
        // struct tcphdr with source and destination ports
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        // 
        const struct tcphdr* tcp_header = (const struct tcphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = tcp_header->th_dport;

      } else if (ipHeader->ip_p == IPPROTO_UDP) {

        // IPPROTO_UDP = 17
        // struct udphdr
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        //
        const struct udphdr* udp_header = (const struct udphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = udp_header->uh_dport;

      } 

      if (ip_types.isDupIP(ipHeader->ip_p, tcp_destination_port) || ip_types.isDupMAC(ipHeader->ip_p, tcp_destination_port)) {
        it++;
        continue;
      }

      // If we get here then we can search for duplicates.
      // Look AFTER the IP header for dupliate IP.

      // Search to see if a MAC is repeated.
      u_int32_t search_len = pkt_header->len - 12;
      const u_int8_t* data_loc = (pkt_data + 12);
      if (find_macs(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it)) {
        ++it;
        continue;
      }

      search_len = pkt_header->len - offset_to_data_ipv4;
      data_loc = pkt_data + offset_to_data_ipv4;
      if (find_ips(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it)) {
        ++it;
        continue;
      }

      // We are done with this packet. No need to search more.
      // The packet does not contain dupliate data!
      ++it;
      continue;

    }

    // TODO: Ethertype IPv6 contain an IPv6 header. Do I understand correctly that I can use
    // The Next Header field to determine the protocol? 



    // Skip any ethernet type that might have a duplicate IP or MAC
    if (ethernet_types.isDupIP(ether_type_int) || ethernet_types.isDupMAC(ether_type_int)) {
      it++;
      continue;
    }

    if (!ethernet_types.isValid(ether_type_int)) {
      if (verbose)
        std::cout << it << " has unexpected ether type " << std::hex << ntohs(ether->ether_type) << std::dec << std::endl;
      // TODO: Check for valid Frame Check Sequence (FCS) as per the flow diagram.
      // We ignore the FCS for now because we do not know if it will be available.

      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;
    }

    // Already filtered out Ether Types that we are ignoring. 
    // Look for a repeated MAC address as follows.
    // The following code makes sure that the type does NOT expect to have duplicate MAC addresses.
    // The flow chart says that the following be done: 
    // If a MAC is dupliated
    //    If a duplicate mac is allowed or expected based on the type, drop the packet and do not check for duplicate IP.
    //    otherwise, save to the anomally file. 
    // This first makes sure that the MAC address is NOT expected to be repeated. 


    // Search to see if a MAC is repeated.
    u_int32_t search_len = pkt_header->len - 12;
    const u_int8_t* data_loc = (pkt_data + 12);
    if (find_macs(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it)) {
      ++it;
      continue;
    }

    if (find_ips(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it)) {
      ++it;
      continue;
    }

    // TODO: In case I need to remember how to deal with an IPv6 header.
    // Remove this.
    /**
    if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {

      if (pkt_header->len < offset_to_data_ipv6) {
        if (verbose)
          std::cout << it << " packet length is too small to contain an IPv6 header (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }
      //
      // What does an IPv6 header look like? 
      //   bits  byte
      //   0-  3  0 version
      //   4- 11    Traffic class
      //  12- 31    Flow Label
      //  32- 47  4 (uint16_t)Payload Length
      //  48- 55  6 (uint8_t)Next Header
      //  56- 63  7 (uint8_t) HOP Limit
      //  64-191  8 (in6_addr / 16 bytes) Source IP
      // 192-288 24 (in6_addr / 16 bytes) Destination IP
      //
      const struct ip6_hdr* ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      const u_int8_t* ip_src = (const u_int8_t*)&(ipHeader->ip6_src);
      const u_int8_t* ip_dst = (const u_int8_t*)&(ipHeader->ip6_dst);

      // TODO: Can I use this next header value as the protocol? 
      // I think I cannot, at least not in all cases. 
      // Types are shown below in the order that they should be processed:
      //  0 - Hop-by-Hop options header
      // 60 - Destination Options Header (first and subsequent destinations)
      // 43 - Routing Header
      // 44 - Fragment Header
      // 51 - Authentication header
      // 50 - Encapsulating Security Payload header.
      // 60 - Destination Options Header (final destination)
      // ?? - Upper-layer header (TODO: what is this? )
      //      MAC addresses are Layer 2. I assume that we are at Layer 3.
      //      I assume that upper layers means layer 4 and above. Layer 4 is TCP. 
      // 59 - There are no headers after this one. 
      u_int8_t next_header = ipHeader->ip6_ctlun.ip6_un1.ip6_un1_nxt;

      //inet_ntop(AF_INET, &(ipHeader->ip6_src), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      //memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      //inet_ntop(AF_INET, &(ipHeader->ip6_dst), ip_addr_max, INET6_ADDRSTRLEN);
      //??unique_ips.insert(std::string(ip_addr_max));
      //memset(ip_addr_max, 0, INET6_ADDRSTRLEN);
      //int offset_to_data_ipv6 = sizeof(struct ether_header) + sizeof(struct ip6_hdr);
      if (find_match(ip_src, 16, (pkt_data + offset_to_data_ipv6), pkt_header->len - offset_to_data_ipv6) || find_match(ip_dst, 4, (pkt_data + offset_to_data_ipv6), pkt_header->len - offset_to_data_ipv6)) {
        // TODO: ??? Is using next_header correct? 
        if (!ip_types.isDupIP(next_header, -1))
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        if (verbose) {
          std::cout << it << " IPv6 address found in payload." << std::endl;
          dump_hex(pkt_data, pkt_header->len);
        }
        ++it;
        continue;
      }

    } **/

    it++;
  }

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
  int ether_type_int;
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
    ether_type_int = ntohs(ether->ether_type);

    if (eth_types_count.find(ether_type_int) == eth_types_count.end()) {
      eth_types_count[ether_type_int] = 1;
    } else {
      ++eth_types_count[ether_type_int];
    }

    if (!ethernet_types.isValid(ether_type_int)) {
      it++;
      continue;
    }
    


    // If we make it here, there should be IPs in the frame.
    if (ether_type_int == ETHERTYPE_IP) {
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
    else if (ether_type_int == ETHERTYPE_IPV6) {
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

  return 0;
}

int write_ip_and_mac_from_pcap(const std::string& pcap_fname, const std::string& out_mac_fname, const std::string& out_ip_fname) {
  mac_addresses.clear();
  ip_addresses.clear();

  pcap_t *pcap_file;
  char *pcap_errbuf;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  u_int done=0;
  int res=1, it=0, i=0, index;

  pcap_file=pcap_open_offline(pcap_fname.c_str(), pcap_errbuf);

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
    mac_addresses.addMacAddress(ether->ether_shost);
    mac_addresses.addMacAddress(ether->ether_dhost);

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
      //const u_int8_t* ip_src = (pkt_data + sizeof(struct ether_header) + 12);
      //const u_int8_t* ip_dst = (pkt_data + sizeof(struct ether_header) + 16);
      ip_addresses.addIpAddress((const u_int8_t*)&(ipHeader->ip_src), true);
      ip_addresses.addIpAddress((const u_int8_t*)&(ipHeader->ip_dst), true);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      ip_addresses.addIpAddress((const u_int8_t*)&(ipHeader->ip6_src), false);
      ip_addresses.addIpAddress((const u_int8_t*)&(ipHeader->ip6_dst), false);
    }

    it++;
  }

  pcap_close(pcap_file);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

  // Write the unique MACs to the output file (stdout by default), and then reset std::cout to the default stdout
  if (out_mac_fname.length() > 0) {
    mac_addresses.write_file(out_mac_fname);
  }

  // Write the unique IPs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_ip_fname.length() > 0) {
    ip_addresses.write_file(out_ip_fname);
  }

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

  ip_fname = "";
  std::string mac_fname;

  struct stat filestat;

  bool verbose_output = false;
  bool create_anomaly_list = false;
  const char* anomaly_fname = nullptr;

  while((arg = getopt(argc, argv, "mpvhr:a:")) != -1){
    switch(arg) {
    case 'v':
      verbose_output = true;
      break;
    case 'a':
      create_anomaly_list=true;
      if(optarg == "-"){anomaly_fname=nullptr;}
      else {anomaly_fname=optarg;}
      break;
    case 'm':
      if(optarg == "-"){mac_fname="";}
      else {mac_fname=optarg;}
      break;
    case 'p':
      if(optarg == "-"){ip_fname="";}
      else {ip_fname=optarg;}
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

  // Lets look at the default IP and MAC file names.
  // the pcap_fname probably ends with ".pcap" so lets
  // create the file name "<base_name>.ip.txt" and "<base_name>.mac.txt"

  std::string pcap_fname_s(pcap_fname);
  std::string pcap_fname_temp(pcap_fname);
  std::string ending(".pcap");
  if (hasEnding(pcap_fname_temp, ending, false)) {
    if (mac_fname.size() == 0) {
      mac_fname = pcap_fname_s.substr(0, pcap_fname_s.size() - 4);
      mac_fname.append("mac.txt");
    }
    if (ip_fname.size() == 0) {
      ip_fname = pcap_fname_s.substr(0, pcap_fname_s.size() - 4);
      ip_fname.append("ip.txt");
    }
  } else {
    if (mac_fname.size() == 0)
      mac_fname = "mac_addresses.txt";
    if (ip_fname.size() == 0)
      ip_fname = "ip_addresses.txt";
  }

  std::cout << "mac_fname = " << mac_fname << std::endl;
  std::cout << "ip_fname = " << ip_fname << std::endl;

  bool create_mac_ip_files = false;
  if(stat(mac_fname.c_str(), &filestat) != 0){
    create_mac_ip_files = true;
  }

  if(stat(ip_fname.c_str(), &filestat) != 0){
    create_mac_ip_files = true;
  }

  if (create_mac_ip_files) {
    std::cout << " creating files " << mac_fname << " and " << ip_fname << std::endl;
    write_ip_and_mac_from_pcap(pcap_fname_s, mac_fname, ip_fname);
  } else {
    std::cout << " reading files " << mac_fname << " and " << ip_fname << std::endl;
    mac_addresses.read_file(mac_fname);
    ip_addresses.read_file(ip_fname);
  }


  if (create_anomaly_list) {
    std::cout << "Ready to go " << std::endl;
    create_heuristic_anomaly_file(ethernet_types, ip_types, pcap_fname, anomaly_fname, verbose_output);
  }

  return 0;
}
