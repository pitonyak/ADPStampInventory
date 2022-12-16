#include "process_pcap.h"

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
#include <net/ethernet.h>  // ETHERTYPE_VLAN 0x8000
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

#include "crc32_x.h"
#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "pcap.h"
#include "utilities.h"
#define ETHERTYPE_MACSEC 0x88e5 /* MACSEC */


// The MAC and IP addresses in this file.
MacAddresses mac_addresses;
IpAddresses ip_addresses;
MacAddresses dest_mac_to_ignore;
std::string extra_name = "anomaly";
std::string output_directory = "";
static int test_flag;

int strip_macsec_vlan_frames(const EthernetTypes &ethernet_types, const std::string &pcap_filename, std::string output_directory, std::string extra_heuristic_name, bool verbose, std::atomic_bool *abort_requested)
{
  std::string anomaly_fname = getHeuristicFileName(pcap_filename, Anomaly_Type, output_directory, extra_heuristic_name);

  pcap_t *pcap_file;
  pcap_dumper_t *dumpfile;
  bool done = false;

  char *pcap_errbuf = nullptr;
  //
  // This structure has three fields:
  // struct timeval ts (time stamp)
  // uint32_t caplen (length of portion present)
  // uint32_t len (length of this packet off wire)
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;

  int res = 1, it = 0;

  // Open the PCAP file!
  pcap_file = pcap_open_offline(pcap_filename.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if (pcap_datalink(pcap_file) != DLT_EN10MB)
  {
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_filename.c_str());
    pcap_close(pcap_file);
    return -1;
  }

  // Open the dump file
  dumpfile = pcap_dump_open(pcap_file, anomaly_fname.c_str());

  if (dumpfile == NULL)
  {
    pcap_close(pcap_file);
    fprintf(stderr, "\nError opening output file\n");
    return -1;
  }

  bool has_header = false;

  const int ether_header_size = sizeof(struct ether_header);          // 14
  const int ip_header_size = sizeof(struct ip);                       // 20
  const int offset_to_data_ipv4 = ether_header_size + ip_header_size; // 34
  const int macsec_offset_etype = 28;                                 // used to skip the first 28 bytes of a frame to check if there is vlan following it
  const int vlan_offset = 4;                                          // used to skip an extra 4 bytes of a frame to account for vlan
  int total_new_data_offset = macsec_offset_etype;

  unsigned int buffer_size = 1024 * 1024 * 2; // 2m
  u_char *newpkt_data = new u_char[buffer_size + 1];
  struct pcap_pkthdr newpkt_header;
  u_char macsec_etype[2];

  // Iterate over every packet in the file and print the MAC addresses
  while (!done)
  {

    // pkt_header contains three fields:
    // struct timeval ts (time stamp) with tv_sec and tv_usec for seconds and micro-seconds I guess.
    // uint32_t caplen (length of portion present)
    // uint32_t len (length of this packet off wire) -
    //
    // The len field is reading fine when referenced directly without converting
    // from network byte order (big-endian) to host endian (little-endian)
    // using ntohl().
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
    res = pcap_next_ex(pcap_file, &pkt_header, &pkt_data);

    if (res == PCAP_ERROR_BREAK)
    {
      fprintf(stderr, "No more packets in savefile. Iteration %d\n", it);
      break;
    }
    if (res != 1)
    {
      fprintf(stderr, "Error reading packet. Iteration %d\n", it);
      continue;
    }

    // The only purpose for this code is to show what is in the packet header
    if (verbose && it < 10)
    {
      time_t ttime = pkt_header->ts.tv_sec;
      tm *local_time = localtime(&ttime);
      std::cout << "index " << it << " caplen:" << pkt_header->caplen << " len:" << pkt_header->len << " ts:" << pkt_header->ts.tv_sec << "." << pkt_header->ts.tv_usec;
      std::cout << " Time: " << 1 + local_time->tm_hour << ":";
      std::cout << 1 + local_time->tm_min << ":";
      std::cout << 1 + local_time->tm_sec << "." << pkt_header->ts.tv_usec << " " << 1 + local_time->tm_mon << "/" << local_time->tm_mday << "/" << 1900 + local_time->tm_year << std::endl;
    }

    // The packet data begins with the Ethernet header, so if we cast to that:
    // uint8_t   ether_dhost [6]  MAC address
    // uint8_t   ether_shost [6]  MAC address
    // uint16_t  etherType        Ethernet type
    ether = (const struct ether_header *)pkt_data;

    // IPv4 address is 4 bytes
    // IPv6 address is 16 bytes
    // MAC address is 6 bytes

    // Some protocols do NOT have an associated ether type;
    // for example, Cisco Discovery Protocol and VLAN Trunking Protocol.
    // This provides a means of ignoring certain broadcast messages.
    //

    int ether_type_int = ntohs(ether->ether_type);

    if (!ethernet_types.isValid(ether_type_int))
    {
      if (verbose || test_flag)
        std::cout << it << " has unexpected ether type " << std::hex << ether_type_int << std::dec << std::endl;

      // Check for valid Frame Check Sequence (FCS) as per the flow diagram.
      // We ignore the FCS for now because we do not know if it will be available.
      // As of 09/29/2022 it is assumed that we will not check for this.

      pcap_dump((u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;
    } else if (ETHERTYPE_MACSEC != ether_type_int) {
      if (verbose)
        std::cout << it << " has no MACSEC layer " << std::hex << ether_type_int << std::dec << std::endl;
      pcap_dump((u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      continue;
    }

    total_new_data_offset = macsec_offset_etype;
    newpkt_header.ts = pkt_header->ts; // timeval will be the same
    const u_char *macsec_etype_data_offset = (pkt_data + macsec_offset_etype);

    if (pkt_header->len > buffer_size) // if frame size is bigger than buffer,
    {
      buffer_size = buffer_size * 2;
      if (buffer_size < pkt_header->len) {
        buffer_size = pkt_header->len + 2;
      }
      delete newpkt_data;
      newpkt_data = new u_char[buffer_size + 1];
    }
    
    memcpy(macsec_etype, macsec_etype_data_offset, 2);
    int macsec_etype_int = ntohs(macsec_etype);
    if( == ETHERTYPE_VLAN)

    // Checke packet length

    // offset_to_search_data = offset_to_data_ipv4;

    // So, what does an IPv4 header look like?
    //  4-bits = [ip_v] Version, so 0100 for IPv4 (byte 0)
    //  4-bits = [ip_hl] HELEN (header length)
    //  8-bits = [ip_tos] Service Type            (byte 1)
    // 16-bits = [ip_len] total Length            (byte 2) [Not used]
    // 16-bits = [ip_id] Identification           (byte 4)
    //  3-bits = Flags                            (byte 6)
    // 13-bits = [lp_off] Fragmentation offset
    //  8-bits = [ip_ttl] Time to live            (byte 8)
    //  8-bits = [ip_p] Protocol                  (byte 9) (17=UDP, 6=TCP) source / destination port.
    // 16-bits = [ip_sum] Header Checksum         (byte 10)
    // 32-bits = [ip_src] Source IP Address       (byte 12)
    // 32-bits = [ip_dst] Destination IP Address  (byte 16)
    // Rest of the Data                           (byte 20)

    //
    // Note that ports are uint16_t stored in network byte order (big-endian)
    // so they must be converted before use.
    //

    // Remember to ntohs
    // before loop:
    //.New packetheader*(struct pcap_pkthdr). sizeof(struct pcap_pkthdr)
    // New newdata*. temp pointer
    // allocate buffer. size? Verify each packet's size
    it++;
  }

  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cerr << "Examined " << it << " packets" << std::endl;

  return 0;
}