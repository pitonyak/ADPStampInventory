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
// If do not have net/if_arp.h, try linux/if_arp.h
#include <net/if_arp.h>
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
#include <ctime>

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
  printf("-v Print verbose output while processing the file.\n");
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
    u_short ethType = ntohs(ether->ether_type);

    // Do I need to do any of this? 
    //mac_to_int(shost, &mac);
    //unique_macs.insert(mac);
    //mac_to_int(dhost, &mac);
    //unique_macs.insert(mac);


    //?? Originally filtered based on this, which allows only IPv4 (0x0800) and IPv6 (0x8gdd)
    /**
    if (ntohs(ether->ether_type) != ETHERTYPE_IP & ntohs(ether->ether_type) != ETHERTYPE_IPV6){
      // If the frame doesn't contain IP headers, skip extracting the IPs. Cause they don't exist.
      it++;
      continue;
    }
    **/

    if (!ethernet_types.isValid(ethType)) {
      if (verbose)
        std::cout << it << " has unexpected ether type " << std::hex << ntohs(ether->ether_type) << std::dec << std::endl;
      // TODO: Check for valid Frame Check Sequence (FCS) as per the flow diagram.
      // We ignore the FCS for now because we do not know if it will be available.

      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;
    }

    // TODO: NO code below here has been tested.

    // TODO: Ethertype IPv4 contains an IPv4 header that contains an entry for Protocol.
    // I assume that ipv4_header->protocol is the Protocol type that is used to determine
    // if a packet may contain a duplicate IP as part of the message. 

    // TODO: Ethertype IPv6 contain an IPv6 header. Do I understand correctly that I can use
    // The Next Header field to determine the protocol? 


    // Already filtered out Ether Types that we are ignoring. 
    // Look for a repeated MAC address as follows.
    // The following code makes sure that the type does NOT expect to have duplicate MAC addresses.
    // The flow chart says that the following be done: 
    // If a MAC is dupliated
    //    If a duplicate mac is allowed or expected based on the type, drop the packet and do not check for duplicate IP.
    //    otherwise, save to the anomally file. 
    // This first makes sure that the MAC address is NOT expected to be repeated. 
    if (find_match(shost, 6, (pkt_data + 12), pkt_header->len - 12) || find_match(dhost, 6, (pkt_data + 12), pkt_header->len - 12)) {
      if (!ethernet_types.isDupMAC(ethType))
        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      if (verbose) {
        std::cout << it << " MAC address found in data: MAC (6 bytes), MAC (6 bytes), ETH TYPE (2 bytes), rest of the payload." << std::endl;
        dump_hex(pkt_data, pkt_header->len);
      }
      ++it;
      continue;
    }

    //std::cout << "size of struct ip = " << sizeof(struct ip) << std::endl;
    // Check for type IP (0x800)
    // The header follows the ether_header
    if (ethType == ETHERTYPE_IP) {
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
      //  8-bits = [ip_p] Protocol                  (byte 9)
      // 16-bits = [ip_sum] Header Checksum         (byte 10)
      // 32-bits = [ip_src] Source IP Address       (byte 12)
      // 32-bits = [ip_dst] Destination IP Address  (byte 16)
      // Rest of the Data                           (byte 20)



      // IPv4 IP Address is four bytes long.
      ip_src = (pkt_data + sizeof(struct ether_header) + 12);
      ip_dst = (pkt_data + sizeof(struct ether_header) + 16);
      //ip_src = (const u_int8_t*)&(ipHeader->ip_src);
      //ip_dst = (const u_int8_t*)&(ipHeader->ip_dst);

      // rest of this code is completely untested!
      // I am supposed to have the PORT, but I do not know how to get the port from this. 
      // int offset_to_data_ipv4 = sizeof(struct ether_header) + sizeof(struct ip);
      if (find_match(ip_src, 4, (pkt_data + offset_to_data_ipv4), pkt_header->len - offset_to_data_ipv4) || find_match(ip_dst, 4, (pkt_data + offset_to_data_ipv4), pkt_header->len - offset_to_data_ipv4)) {
        if (!ip_types.isDupIP(ipHeader->ip_p, -1))
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        if (verbose) {
          std::cout << it << " IPv4 address found in payload." << std::endl;
          dump_hex(pkt_data, pkt_header->len);
        }
        ++it;
        continue;
      }      

      // Turn the raw src and dst IPs in the packet into human-readable
      //   IPs and insert them into the set
      // Be sure to clear the char* buffer each time so that the end
      //   of the IP will always be correctly null-terminated
      // If we don't do this, we risk keeping junk from the previous IP
      //   that may have been longer than the current IP.
      //inet_ntop(AF_INET, &(ipHeader->ip_src), ip_addr_max, INET_ADDRSTRLEN);
      //unique_ips.insert(std::string(ip_addr_max));
      //memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

      //inet_ntop(AF_INET, &(ipHeader->ip_dst), ip_addr_max, INET_ADDRSTRLEN);
      //unique_ips.insert(std::string(ip_addr_max));
      //memset(ip_addr_max, 0, INET6_ADDRSTRLEN);

    } else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {

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

    } else if (ntohs(ether->ether_type) == ETHERTYPE_LOOPBACK) {
      // Loopback test is 0x9000
      //
      // What does the header look like? 
      if (pkt_header->len < (sizeof(struct ether_header) + 0)) {
        if (verbose)
          std::cout << it << " packet length is too small to contain a loopback header (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }
      //
      // What does an loopback header look like? 
      //
      // TODO: ???
   

    } else if (ntohs(ether->ether_type) == ETHERTYPE_ARP) {
      // ARP is 806
      if (pkt_header->len < (sizeof(struct ether_header) + sizeof(struct arphdr))) {
        if (verbose)
          std::cout << it << " packet length is too small to contain an ARP header (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }
      //
      // What does an ETHERTYPE_ARP header look like? 
      // This should be eight bytes long as taken from if_arp.h. 
      //
      // unsigned short int ar_hrd;  Format of hardware address. 1 for Ethernet
      // unsigned short int ar_pro;  protocol type             . See if_arp.h, but, ARP, RARP, etc. 
      // unsigned char ar_hln;       Length of hardware address. 6 for Ethernet
      // unsigned char ar_pln;       Length of protocol address. 4 for IPv4
      // unsigned short int ar_op;   ARP opcode (command). 1:request, 2:response
      //
      // Next bits are NOT defined in the header file, but:
      // sender hardware address (HMAC / hardware of source)
      // sender IP address       (Layer 3 address of source)
      // target hardware address (used in RARP request)
      // target IP address       (Used in ARP when response includes layer 3 addresses)
      //

      // TODO: ??? is the sender and target always filled in? are the MAC and IP always filled in?
      // 
      const struct arphdr* arpHeader = (struct arphdr*)(pkt_data + sizeof(struct ether_header));
      const int arp_header_size = sizeof(struct arphdr) + 2 * arpHeader->ar_hln + 2 * arpHeader->ar_pln;

      if (pkt_header->len < (sizeof(struct ether_header) + arp_header_size)) {
        if (verbose)
          std::cout << it << " packet length is too small to contain an ARP header with addresses (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }

      const u_int8_t* mac_src = (const u_int8_t*)(arpHeader + sizeof(struct arphdr));
      const u_int8_t* ip_src  = mac_src + arpHeader->ar_hln;
      const u_int8_t* mac_dst = ip_src  + arpHeader->ar_pln;
      const u_int8_t* ip_dst  = mac_dst + arpHeader->ar_hln;
      const int offset_to_data_arp = sizeof(struct ether_header) + arp_header_size;

      if (find_match(mac_src, arpHeader->ar_hln, (pkt_data + offset_to_data_arp), pkt_header->len - offset_to_data_arp) || find_match(mac_dst, arpHeader->ar_hln, (pkt_data + offset_to_data_arp), pkt_header->len - offset_to_data_arp)) {
        if (!ethernet_types.isDupMAC(ethType))
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        if (verbose) {
          std::cout << it << " Dupliate MAC address found in ARP." << std::endl;
          dump_hex(pkt_data, pkt_header->len);
        }
        ++it;
        continue;
      }      
      if (find_match(ip_src, arpHeader->ar_pln, (pkt_data + offset_to_data_arp), pkt_header->len - offset_to_data_arp) || find_match(ip_dst, arpHeader->ar_pln, (pkt_data + offset_to_data_arp), pkt_header->len - offset_to_data_arp)) {
        if (!ethernet_types.isDupIP(ethType))
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        if (verbose) {
          std::cout << it << " Dupliate IP address found in ARP." << std::endl;
          dump_hex(pkt_data, pkt_header->len);
        }
        ++it;
        continue;
      }      

    } else if (ntohs(ether->ether_type) == 0x08847) {
      // MPLS Unicast traffic is 0x08847 ==  34887
      //
      // The MPLS header is a 4-byte header, 
      // located immediately before the IP header. 
      // Many people simply refer to the MPLS header as the MPLS label, 
      // but the label is actually a 20-bit field in the MPLS header. 
      // You may also see this header referenced as an MPLS shim header. 
      // 
      // MPLS Header is as follows: 
      //
      // BITS   Byte 
      // 20       0  Label - Identifies the LSP portion of the label switched path
      // 3           Used for QoS marking. No longer used
      // 1           Flag when 1 means the label immediately precedes the IP header
      // 8        3  TTL fields. Same as the IP header's TTL field.

      // TODO: ??? So an MPLS packet contains the MPLS header (4 bytes) followed by an IP header? 

      if (pkt_header->len < (sizeof(struct ether_header) + 4 + sizeof(struct ip))) {
        if (verbose)
          std::cout << it << " packet length is too small to contain an MPLS header (" << pkt_header->len << ")" << std::endl;

        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        continue;
      }

      struct ip* ipHeader = (struct ip*)(pkt_data + sizeof(struct ether_header) + 4);

      // TODO: So now do I dupliate the code for an ip header? 
  
    } else {
      // This type is NOT implemented but is configured as OK in eth_types.txt
      if (verbose)
        std::cout << it << " This ethernet type is allowed in eth_types.txt but not supported in code. Type: " << std::hex << ntohs(ether->ether_type) << std::dec << std::endl;
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

  bool verbose_output = false;
  bool create_anomaly_list = false;
  const char* anomaly_fname = nullptr;

  while((arg = getopt(argc, argv, "vhr:a:")) != -1){
    switch(arg) {
    case 'v':
      verbose_output = true;
      break;
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
    create_heuristic_anomaly_file(ethernet_types, ip_types, pcap_fname, anomaly_fname, verbose_output);
  }

  return 0;
}
