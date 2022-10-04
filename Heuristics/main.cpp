#include <algorithm>
#include <arpa/inet.h>
#include <cstdint>
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

#include "ahocorasickbinary.h"
#include "crc32_x.h"
#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "pcap.h"
#include "utilities.h"

// The MAC and IP addresses in this file.
MacAddresses mac_addresses;
IpAddresses ip_addresses;

enum SearchTypeEnum { forward_search, backward_search, aho_corasick_binary };

void usage(){
  printf("Usage:\n");
  printf("-h Print this help.\n");
  printf("-v Print verbose output while processing the file.\n");
  printf("-r <path to input pcap file>: This PCAP file will be read for all MAC addresses and IP addresses\n");
  printf("-a <path to generated anomaly pcap>: Where to write the anomaly list. This triggers the creation of the anomaly list.\n");
  printf("-p <path to IP output filename, default 'ip_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all IP addresses found in the input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("-m <path to MAC output filename, default 'mac_addresses.txt'>: This OPTIONAL file will be populated with the unique, human-readable versions of all Ethernet MAC addresses input PCAP file. If this option is not given, stdout will be used. If '-' is given as the output file, MAC addresses will be printed to stdout.\n");
  printf("\n");
}

//**************************************************************************
//! Generate a vector from a set.
/*!
 * \param [in] x Set of pointers to copy.
 * 
 * \returns a vector with the same values. The vector does NOT own the pointers and they are simply assumed to exist.
 *
 ***************************************************************************/
std::vector<uint8_t *>* toVector(const std::set<uint8_t *>& x) {
  std::vector<uint8_t *>* v = new std::vector<uint8_t *>();
  v->reserve(x.size());
  for (auto const &ptr: x) {
    v->push_back(ptr);
  }
  return v;
}

std::unique_ptr<std::vector<int>> getConstWordLengthVector(const int wordLength, const int n) {
  std::unique_ptr<std::vector<int>> v(new std::vector<int>());
  v->reserve(n);
  for (int i=0; i< n; ++i) {
    v->push_back(wordLength);
  }
  return v;
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
//! Dump the packet and print verbose message if appropriate.
/*!
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
void local_pcap_dump(pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it, const char* message) {

  pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
  if (verbose) {
    std::cout << it;
    if (message != nullptr) {
      std::cout << " " << message;
    }
    std::cout << std::endl;
    dump_hex(pkt_data, pkt_header->len);
  }

}

//**************************************************************************
//! Search for a repeated IP address. If a duplicate is found, the packet is saved.
/*!
 * 
 * \param [in] a Initialized object ready to search.
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
 * \param [in] message. Send to the dump routine so a message can be logged if in verbose mode.
 * 
 * \returns True if a duplicate IP is found.
 *
 ***************************************************************************/
bool aho_search(const AhoCorasickBinary& a, const uint8_t* search_start_loc, uint32_t search_len, pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it, const char* message) {

  if (a.findFirstMatch(search_start_loc, search_len) >= 0) {

    local_pcap_dump(dumpfile, pkt_header, pkt_data, verbose, it, message);
    return true;

  }
  return false;
}

//**************************************************************************
//! Search for a repeated MAC address. If a duplicate is found, the packet is saved.
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
bool find_macs(const uint8_t* search_start_loc, uint32_t search_len, pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it, SearchTypeEnum search_type, const AhoCorasickBinary& a) {

  if (search_type == aho_corasick_binary) {
    // aho search dumps the file if a match is found.
    return aho_search(a, search_start_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, "MAC address found in data");
  } else {
    for (auto const &a_mac: mac_addresses.m_unique_macs) {
      if ((search_type == forward_search  && find_match(a_mac, 6, search_start_loc, search_len)) || 
          (search_type == backward_search && reverse_match(a_mac, 6, search_start_loc, search_len))) {
        local_pcap_dump(dumpfile, pkt_header, pkt_data, verbose, it, "MAC address found in data");
        return true;
      }
    }
  }
  return false;
}

//**************************************************************************
//! Search for a repeated IP address. If a duplicate is found, the packet is saved.
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
 * \returns True if a duplicate IP is found.
 *
 ***************************************************************************/
bool find_ips(const uint8_t* search_start_loc, uint32_t search_len, pcap_dumper_t *dumpfile, struct pcap_pkthdr *pkt_header, const u_char *pkt_data, bool verbose, int it, SearchTypeEnum search_type, const AhoCorasickBinary& ipv4, const AhoCorasickBinary& ipv6) {

  if (search_type == aho_corasick_binary) {
    // aho search dumps the file if a match is found.
    return aho_search(ipv4, search_start_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, "IPv4 address found in data") || 
           aho_search(ipv6, search_start_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, "IPv6 address found in data");

  } else {
    for (auto const &an_ip: ip_addresses.m_unique_ipv4) {
      if ((search_type == forward_search && find_match(an_ip, 4, search_start_loc, search_len)) || 
          (search_type == backward_search && reverse_match(an_ip, 4, search_start_loc, search_len))) {

        local_pcap_dump(dumpfile, pkt_header, pkt_data, verbose, it, "IPv4 address found in data");
        return true;
      }
    }

    for (auto const &an_ip: ip_addresses.m_unique_ipv6) {
      if ((search_type == forward_search && find_match(an_ip, 16, search_start_loc, search_len)) || 
          (search_type == backward_search && reverse_match(an_ip, 16, search_start_loc, search_len))) {

        local_pcap_dump(dumpfile, pkt_header, pkt_data, verbose, it, "IPv6 address found in data");
        return true;
      }
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

  // Aho-Corasick is significantly faster that forward or backward search.
  SearchTypeEnum search_type = aho_corasick_binary;
  //search_type = backward_search;
  //search_type = forward_search;

  AhoCorasickBinary search_ipv4;
  AhoCorasickBinary search_ipv6;
  AhoCorasickBinary search_macs;

  if (search_type == aho_corasick_binary) {
    std::unique_ptr<std::vector<uint8_t *>> words_ipv4 = ip_addresses.toVector(true);
    std::unique_ptr<std::vector<uint8_t *>> words_ipv6 = ip_addresses.toVector(false);
    std::unique_ptr<std::vector<uint8_t *>> words_macs = mac_addresses.toVector();
    std::unique_ptr<std::vector<int>> lengths_ipv4 = getConstWordLengthVector( 4, ip_addresses.m_unique_ipv4.size());
    std::unique_ptr<std::vector<int>> lengths_ipv6 = getConstWordLengthVector(16, ip_addresses.m_unique_ipv6.size());
    std::unique_ptr<std::vector<int>> lengths_macs = getConstWordLengthVector( 6, mac_addresses.m_unique_macs.size());
    std::cout << "Initializing ipv4 search" << std::endl;

    search_ipv4.buildMatchingMachine(*words_ipv4.get(), *lengths_ipv4.get());
    std::cout << "Initializing ipv6 search" << std::endl;
    search_ipv6.buildMatchingMachine(*words_ipv6.get(), *lengths_ipv6.get());
    std::cout << "Initializing macs search" << std::endl;
    search_macs.buildMatchingMachine(*words_macs.get(), *lengths_macs.get());

    // We do not need to keep or remember the words because we do not track what was found.
    // If we wanted to remember that, then we would need to keep the words vectors.
    // In other words, move the words out of the loop.
  }

  char *pcap_errbuf = nullptr;
  //
  // This structure has three fields:
  // struct timeval ts (time stamp)
  // uint32_t caplen (length of portion present)
  // uint32_t len (length of this packet off wire)
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;

  int res=1, it=0;

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
  [[maybe_unused]] const int offset_to_data_ipv6 = sizeof(struct ether_header) + sizeof(struct ip6_hdr);

  // Iterate over every packet in the file and print the MAC addresses
  while(!done){
    // pkt_header contains three fields:
    // struct timeval ts (time stamp) with tv_sec and tv_usec for seconds and micro-seconds I guess.
    // uint32_t caplen (length of portion present)
    // uint32_t len (length of this packet off wire)
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

    // The packet data begins with the Ethernet header, so if we cast to that:
    // uint8_t   ether_dhost [6]  MAC address
    // uint8_t   ether_shost [6]  MAC address
    // uint16_t  etherType        Ethernet type
    ether = (const struct ether_header*)pkt_data;

    // IPv4 address is 4 bytes
    // IPv6 address is 16 bytes
    // MAC address is 6 bytes

    // Extract the frame MACs and put them into the set for uniqueness discovery
    //const uint8_t *shost = ether->ether_shost;
    //const uint8_t *dhost = ether->ether_dhost;
    int ether_type_int = ntohs(ether->ether_type);


    if (!ethernet_types.isValid(ether_type_int)) {
      if (verbose)
        std::cout << it << " has unexpected ether type " << std::hex << ntohs(ether->ether_type) << std::dec << std::endl;
      
      // Check for valid Frame Check Sequence (FCS) as per the flow diagram.
      // We ignore the FCS for now because we do not know if it will be available.
      // As of 09/29/2022 it is assumed that we will not check for this.

      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;

    }

    // Check for type IP (0x800)
    // This type is skipped for some types, but not others.
    // Set the eth_types.txt file so that it does NOT indicate that
    // this type has copies of the IP or MAC and rely on this check instead.
    // The header follows the ether_header
    if (ether_type_int == ETHERTYPE_IP) {

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
      int tcp_source_port = -1;
      std::string proto = "Unknown";
      if (static_cast<int>(ipHeader->ip_p) == static_cast<int>(IPPROTO_TCP)) {

        // IPPROTO_TCP = 6
        // struct tcphdr with source and destination ports
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        // 
        // Port 161 could be in source or destination. TODO: ???? If source is 161, special case!
        //
        const struct tcphdr* tcp_header = (const struct tcphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = tcp_header->th_dport;
        tcp_source_port = tcp_header->th_sport;
        proto = "TCP";

      } else if (static_cast<int>(ipHeader->ip_p) == static_cast<int>(IPPROTO_UDP)) {

        // IPPROTO_UDP = 17
        // struct udphdr
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        //
        const struct udphdr* udp_header = (const struct udphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = udp_header->uh_dport;
        tcp_source_port = udp_header->uh_sport;
        proto = "UDP";
      }

      // If we get here then we can search for duplicates.
      // Look AFTER the IP header for dupliate IP.

      // Search to see if a MAC is repeated.
      bool may_have_dup = ip_types.isDupMAC(ipHeader->ip_p, tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupMAC(ipHeader->ip_p, tcp_source_port));
      //std::cout << "May MAC Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        uint32_t search_len = pkt_header->len - 12;
        const uint8_t* data_loc = (pkt_data + 12);
        if (find_macs(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_macs)) {
          ++it;
          continue;
        }
      }

      may_have_dup = ip_types.isDupIP(ipHeader->ip_p, tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupIP(ipHeader->ip_p, tcp_source_port));
      //std::cout << "May  IP Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        uint32_t search_len = pkt_header->len - offset_to_data_ipv4;
        const uint8_t* data_loc = pkt_data + offset_to_data_ipv4;
        if (find_ips(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_ipv4, search_ipv6)) {
          ++it;
          continue;
        }
      }

      // We are done with this ipv4 packet. No need to search more.
      // The packet does not contain dupliate data!
      ++it;
      continue;
    }

    // Done processing IPv4. We are here, so this is NOT IPv4 so check for IPv6.

    // In case I need to remember how to deal with an IPv6 header.
    // Remove this.
    /**
    if (ether_type_int == ETHERTYPE_IPV6) {

      //
      // What does an IPv6 header look like? 
      //   bits  byte
      //   0-  3  0 version
      //   4- 11    Traffic class
      //  12- 31    Flow Label
      //  32- 47  4 (uint16_t) Payload Length
      //  48- 55  6 (uint8_t) Next Header
      //  56- 63  7 (uint8_t) HOP Limit
      //  64-191  8 (in6_addr / 16 bytes) Source IP
      // 192-288 24 (in6_addr / 16 bytes) Destination IP
      //         40 This points just past the header!
      //
      // A comment on Next Header:
      // This is either the type of Extension Header, or, if the Extension Header is not present,
      // it indicates the Upper Layer PDU. The values for the type of Upper Layer PDU are same as IPv4’s.
      //
      const struct ip6_hdr* ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      //const uint8_t* ip_src = (const uint8_t*)&(ipHeader->ip6_src);
      //const uint8_t* ip_dst = (const uint8_t*)&(ipHeader->ip6_dst);
      uint8_t next_header = ipHeader->ip6_ctlun.ip6_un1.ip6_un1_nxt;

      // The next header value is the protocol from IPv4 (see https://notes.shichao.io/tcpv1/ch5/)
      // The values below are specific to IPv6, but this also includes the standard types in IPv4.
      // Types are shown below in the order that they should be processed assuming an IPv6 type.
      //  0 - Hop-by-Hop options header (struct ip6_hbh)
      // 60 - Destination Options Header (first and subsequent destinations) (struct ip6_dest)
      // 43 - Routing Header (struct ip6_rthdr or maybe struct ip6_rthdr0)
      // 44 - Fragment Header (struct ip6_frag)
      // 51 - Authentication header
      // 50 - Encapsulating Security Payload header.
      // 60 - Destination Options Header (final destination)
      // ?? - Upper-layer header (TODO: what is this? )
      //      MAC addresses are Layer 2. I assume that we are at Layer 3.
      //      I assume that upper layers means layer 4 and above. Layer 4 is TCP. 
      // 59 - There are no headers after this one.

      //
      // Extension headers, along with headers of higher-layer protocols such as TCP or UDP, 
      // are chained together with the IPv6 header to form a cascade of headers. (see https://notes.shichao.io/tcpv1/ch5/)
      // The Next Header field in each header indicates the type of the subsequent header, 
      // which could be an IPv6 extension header or some other type. 
      // The value of 59 indicates the end of the header chain.
      //
      // For 17/UDP (chapter 10 https://notes.shichao.io/tcpv1/ch10/#udp-and-ipv6) 
      // For  6/TCP (chapters 13-17 https://notes.shichao.io/tcpv1/ch10/)
      // I think that one of the UDP ports is at byte 54 so probably past an DLT_EN10MB extension header.


      //int offset_to_data_ipv6 = sizeof(struct ether_header) + sizeof(struct ip6_hdr);

      int tcp_destination_port = -1;
      int tcp_source_port = -1;
      std::string proto = "Unknown";
      uint32_t final_data_offset = offset_to_data_ipv6;

      if (static_cast<int>(next_header) == static_cast<int>(IPPROTO_TCP)) {

        // IPPROTO_TCP = 6
        // struct tcphdr with source and destination ports
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        // 
        // Port 161 could be in source or destination. TODO: ???? If source is 161, special case!
        //

        const struct tcphdr* tcp_header = (const struct tcphdr*)(pkt_data + offset_to_data_ipv6);
        final_data_offset += sizeof(struct tcphdr);
        tcp_destination_port = tcp_header->th_dport;
        tcp_source_port = tcp_header->th_sport;
        proto = "TCP";

      } else if (static_cast<int>(next_header) == static_cast<int>(IPPROTO_UDP)) {

        // IPPROTO_UDP = 17
        // struct udphdr
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        //
        const struct udphdr* udp_header = (const struct udphdr*)(pkt_data + offset_to_data_ipv6);
        final_data_offset += sizeof(struct udphdr);
        tcp_destination_port = udp_header->uh_dport;
        tcp_source_port = udp_header->uh_sport;
        proto = "UDP";
      }

      // If we get here then we can search for duplicates.
      // Look AFTER the IP header for dupliate IP.
      if (pkt_header->len < final_data_offset) {
        local_pcap_dump(dumpfile, pkt_header, pkt_data, verbose, it, "Error, packet data length is less than offset to packet data.");
        ++it;
        continue;
      }

      // Search to see if a MAC is repeated.
      bool may_have_dup = ip_types.isDupMAC(static_cast<int>(next_header), tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupMAC(static_cast<int>(next_header), tcp_source_port));
      //std::cout << "May MAC Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        uint32_t search_len = pkt_header->len - final_data_offset;
        const uint8_t* data_loc = (pkt_data + final_data_offset);
        if (find_macs(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_macs)) {
          ++it;
          continue;
        }
      }

      may_have_dup = ip_types.isDupIP(static_cast<int>(next_header), tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupIP(static_cast<int>(next_header), tcp_source_port));
      //std::cout << "May  IP Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        uint32_t search_len = pkt_header->len - final_data_offset;
        const uint8_t* data_loc = (pkt_data + final_data_offset);
        if (find_ips(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_ipv4, search_ipv6)) {
          ++it;
          continue;
        }
      }

      // We are done with this ipv6 packet. No need to search more.
      // The packet does not contain dupliate data!
      ++it;
      continue;
      // End of IPv6 testing
    }
    **/


    // Already filtered out (saved) Ether Types that are not considered valid.
    // Look for a repeated MAC address as follows.
    // The following code makes sure that the type does NOT expect to have duplicate MAC addresses.
    // The flow chart says that the following be done: 
    // If a MAC is dupliated
    //    If a duplicate mac is allowed or expected based on the type, drop the packet and do not check for duplicate IP.
    //    otherwise, save to the anomally file. 
    // This first makes sure that the MAC address is NOT expected to be repeated. 

    // Search to see if a MAC is repeated.
    uint32_t search_len = pkt_header->len - 12;
    const uint8_t* data_loc = (pkt_data + 12);
    if (!ethernet_types.isDupMAC(ether_type_int)) {
      if (find_macs(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_macs)) {
        ++it;
        continue;
      }
    }

    if (!ethernet_types.isDupIP(ether_type_int)) {
      if (find_ips(data_loc, search_len, dumpfile, pkt_header, pkt_data, verbose, it, search_type, search_ipv4, search_ipv6)) {
        ++it;
        continue;
      }
    }

    // Done with all processing for this packet.

    it++;
  }


  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cerr << "Examined "<< it <<" packets"<<std::endl;

  return 0;
}


int write_ip_and_mac_from_pcap(const std::string& pcap_fname, const std::string& out_mac_fname, const std::string& out_ip_fname) {
  mac_addresses.clear();
  ip_addresses.clear();

  pcap_t *pcap_file;
  char *pcap_errbuf = nullptr;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  u_int done=0;
  int res=1, it=0;

  pcap_file=pcap_open_offline(pcap_fname.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    std::cerr << "PCAP file " << pcap_fname << "is not an Ethernet capture\n" << std::endl;
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
      //const uint8_t* ip_src = (pkt_data + sizeof(struct ether_header) + 12);
      //const uint8_t* ip_dst = (pkt_data + sizeof(struct ether_header) + 16);
      ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip_src), true, false);
      ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip_dst), true, false);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip6_src), false, false);
      ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip6_dst), false, false);
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

bool startsWith(const char* s, char c) {
  return (s != nullptr && s[0] == c);
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

  //std::string ip_fname = "ip_types.txt";
  std::string new_ip_fname = "ip_protocols.txt";
  std::string new_ip_ports_fname = "ip_protocol_ports.txt";

  IPTypes ip_types;
  //ip_types.readProtocols(ip_fname, true, 10);
  ip_types.readProtocols(new_ip_fname, false, 10);
  ip_types.readProtocolPorts(new_ip_ports_fname);
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

  std::string ip_fname;
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
      //??if(optarg == "-"){anomaly_fname=nullptr;}
      if (startsWith(optarg, '-')){anomaly_fname=nullptr;}
      else {anomaly_fname=optarg;}
      break;
    case 'm':
      //if(optarg == "-"){mac_fname="";}
      if (startsWith(optarg, '-')){mac_fname="";}
      else {mac_fname=optarg;}
      break;
    case 'p':
      //if(optarg == "-"){ip_fname="";}
      if (startsWith(optarg, '-')){ip_fname="";}
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
  std::string pcap_extension = getFileExtension(pcap_fname_temp);
  if (pcap_extension.compare(".pcap") == 0 || pcap_extension.compare(".cap") == 0 ) {
    if (mac_fname.size() == 0) {
      mac_fname = pcap_fname_s.substr(0, pcap_fname_s.size() - pcap_extension.length() + 1);
      mac_fname.append("mac.txt");
    }
    if (ip_fname.size() == 0) {
      ip_fname = pcap_fname_s.substr(0, pcap_fname_s.size() - pcap_extension.length() + 1);
      ip_fname.append("ip.txt");
    }
  } else {
    if (mac_fname.size() == 0)
      mac_fname = "mac_addresses.txt";
    if (ip_fname.size() == 0)
      ip_fname = "ip_addresses.txt";
  }
  
  if (!isPathExist(pcap_fname_s, true, false, false, false)) {
    std::cout << "PCAP file does not exist: " << pcap_fname_s << std::endl;
    return -1;
  }
  if (!isPathExist(pcap_fname_s, true, false, true, false)) {
    std::cout << "Do not have read permission on PCAP file: " << pcap_fname_s << std::endl;
    std::cout << "This may fail to read the PCAP file" << std::endl;
    //return -1;
  }

  bool create_mac_ip_files = !isPathExist(mac_fname, true, false, false, false) || !isPathExist(ip_fname, true, false, false, false);

  if (create_mac_ip_files) {
    std::string path = getDirectoryFromFilename(mac_fname);
    if (!isPathExist(path, false, true, true, true)) {
      std::cout << "Cannot read/write to directory where the MAC file will be created: " << path << std::endl;
      std::cout << "This may fail to create the MAC file" << std::endl;
      //return -1;
    }
    path = getDirectoryFromFilename(ip_fname);
    if (!isPathExist(path, false, true, true, true)) {
      std::cout << "Cannot read/write to directory where the IP file will be created: " << ip_fname << std::endl;
      std::cout << "This may fail to create the IP file" << std::endl;
      //return -1;
    }

    std::cout << " creating files " << mac_fname << " and " << ip_fname << std::endl;
    write_ip_and_mac_from_pcap(pcap_fname_s, mac_fname, ip_fname);
  } else {
    if (!isPathExist(mac_fname, true, false, true, false) || !isPathExist(ip_fname, true, false, true, false)) {
      std::cout << "ERROR: Cannot read " << mac_fname << " or " << ip_fname << std::endl;
      std::cout << "This may fail to create the MAC and IP files" << std::endl;
      //return -1;
    }
    std::cout << " reading files " << mac_fname << " and " << ip_fname << std::endl;
    mac_addresses.read_file(mac_fname);
    ip_addresses.read_file(ip_fname);
  }

  if (create_anomaly_list) {
    if (anomaly_fname == nullptr) {
      std::cout << "Anomaly filename cannot be NULL" << std::endl;
      return -1;
    }
    if (isPathExist(anomaly_fname, true, false, false, false)) {
      std::cout << "Anomaly file will be over-written: " << anomaly_fname << std::endl;
    }
    std::string path = getDirectoryFromFilename(anomaly_fname);
    if (!isPathExist(path, false, true, true, true)) {
      std::cout << "Cannot read/write to directory where the anomaly file will be created: " << path << std::endl;
      std::cout << "This may error out" << std::endl;
      //return -1;
    }

    std::cout << "Creating Anomaly File" << std::endl;
    create_heuristic_anomaly_file(ethernet_types, ip_types, pcap_fname, anomaly_fname, verbose_output);
  }

  return 0;
}
