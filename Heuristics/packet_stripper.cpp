
#include "process_pcap.h"

#include <getopt.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <net/ethernet.h> // struct ether_header
#include <netinet/ip.h>   // struct ip
#include <netinet/ip6.h>  // struct ip6_hdr
#include <netinet/tcp.h>  // struct tcphdr
#include <netinet/udp.h>  // struct udphdr

#include "pcap.h"
#include "utilities.h"

static int strip_8847 = 0; // MPLS Unicast
static int strip_8848 = 0; // MPLS Multicast
static int strip_8A88 = 0; // Q-in-Q / Service Tag
static int strip_8100 = 0; // Customer Tag
static int strip_88E5 = 0; // macsec

bool strip_mpls(uint32_t packet_len, const u_char *pkt_data, uint32_t mpls_start, uint32_t& mpls_len, int& new_ethertype) {

  mpls_len = 0;
  new_ethertype = 0;

  // Assume this is an MPLS packet.
  // 20 for the label
  // 03 experimental bits
  // 01 bit (bottom of stack flag)
  // 08 bits Time To Live

  // Bottom of stack flag with a value of 0 means
  // that there is another MPLS flag.
  uint8_t bos;
  do {
    std::cout << "MPLS Layer." << std::endl;
    // Deal with truncated MPLS.
    if (packet_len < (mpls_start + mpls_len + 4)) {
      std::cout << "Truncated Packet(mpls) at length " << (mpls_len + 4) << std::endl;
      return false;
    }
    //uint32_t label = (pkt_data[mpls_start + mpls_len + 2] >> 4) + pkt_data[mpls_start + mpls_len + 1] * 16 + pkt_data[mpls_start + mpls_len] * 4096;
    //std::cout << "MPLS Layer label = " << std::hex << label << std::dec << " or " << label << std::endl;
    bos = pkt_data[mpls_start + mpls_len + 2] & 0x01;
    mpls_len += 4;
  } while (bos == 0);

  if (packet_len < (mpls_start + mpls_len)) {
    std::cout << "Truncated Packet(mpls) at length " << mpls_len << std::endl;
    return false;
  }

  // 'Guess' the next protocol. This can result in false positives.
  uint8_t ip_ver = pkt_data[mpls_start + mpls_len] >> 4;
  switch (ip_ver) {
    case 0x04:
      new_ethertype = 0x0800; break;
    case 0x06:
      new_ethertype = 0x86DD; break;
    default:
      std::cout << "Warning, returning ethernet type 0 from MPLS" << std::endl;
      new_ethertype = 0;
  }

  return true;
}

int process_mpls(const std::string& pcap_filename, std::string output_directory, std::string extra_heuristic_name, std::atomic_bool* abort_requested) {

  std::string output_fname = getHeuristicFileName(pcap_filename, Anomaly_Type, output_directory, extra_heuristic_name);
  if (isPathExist(output_fname, true, false, false, false)) {
    std::cout << "Output file will be over-written: " << output_fname << std::endl;
  }

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
  struct pcap_pkthdr pkt_header_copy;

  const u_char *pkt_data;
  uint32_t data_copy_len = 1024 * 1024 * 4;
  u_char *pkt_data_copy = new u_char[data_copy_len + 1];

  struct ether_header *ether;

  int res=1, it=1;
  int num_mpls = 0;

  // Open the PCAP file!
  pcap_file=pcap_open_offline(pcap_filename.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_filename.c_str());
    pcap_close(pcap_file);
    return -1;
  }

  // Open the dump file
  dumpfile = pcap_dump_open(pcap_file, output_fname.c_str());

  if(dumpfile==NULL)
  {
    pcap_close(pcap_file);
    fprintf(stderr,"\nError opening output file\n");
    return -1;
  }

  const int ether_header_size = sizeof(struct ether_header); // 14

  // Iterate over every packet in the file and print the MAC addresses
  while(!done && (abort_requested == nullptr || !abort_requested->load()) ){
    // pkt_header contains three fields:
    // struct timeval ts (time stamp) with tv_sec and tv_usec for seconds and micro-seconds I guess.
    // uint32_t caplen (length of portion present)
    // uint32_t len (length of this packet off wire) - 
    //
    // The len field is reading fine when referenced directly without converting 
    // from network byte order (big-endian) to host endian (little-endian)
    // using ntohl().
    //
    // pkt_data points to the data.
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);

    if(res == PCAP_ERROR_BREAK){
      fprintf(stderr, "No more packets in savefile. Iteration %d\n", it);
      break;
    }
    if(res != 1){
      fprintf(stderr, "Error reading packet. Iteration %d\n", it);
      continue;
    }

    // The packet data begins with the Ethernet header, so if we cast to that:
    // uint8_t   ether_dhost [6]  MAC address
    // uint8_t   ether_shost [6]  MAC address
    // uint16_t  etherType        Ethernet type
    ether = (struct ether_header*)pkt_data;

    // IPv4 address is 4 bytes
    // IPv6 address is 16 bytes
    // MAC address is 6 bytes

    int ether_type_int = ntohs(ether->ether_type);

    if (ether_type_int == 0x8847) {
      // Strip MPLS and move on!
      uint32_t mpls_start = ether_header_size;
      uint32_t mpls_len = 0;
      int new_ethertype = 0;
      bool ok = strip_mpls(pkt_header->len, pkt_data, mpls_start, mpls_len, new_ethertype);
      std::cout << "Packet " << it << " MPLS " << ok << " mpls_len:" << mpls_len << " new_ethertype:" << std::hex << new_ethertype << std::dec << std::endl;
      if (ok) {
        if (pkt_header->len > data_copy_len) {
          delete[] pkt_data_copy;
          data_copy_len = pkt_header->len * 2;
          pkt_data_copy = new u_char[data_copy_len + 1];
        }
        memcpy( &pkt_header_copy, pkt_header, sizeof(struct pcap_pkthdr) );
        pkt_header_copy.len -= mpls_len;
        pkt_header_copy.caplen = pkt_header_copy.len;

        memcpy( pkt_data_copy, pkt_data,  mpls_start);
        memcpy( pkt_data_copy + mpls_start, pkt_data + mpls_start + mpls_len,  pkt_header->len - mpls_start - mpls_len);
        ether = (struct ether_header*)pkt_data_copy;
        ether->ether_type = htons(new_ethertype);
        pcap_dump( (u_char *)dumpfile, &pkt_header_copy, pkt_data_copy);
      }

      ++num_mpls;
    } else {
      // Not MPLS
      //std::cout << "Packet " << it << " is not MPLS type is " << std::hex << ether_type_int << std::dec << std::endl;
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
    }
    // Done with all processing for this packet.
    ++it;
  }

  delete[] pkt_data_copy;
  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cout << " Examined "<< (it - 1) <<" packets in file " << pcap_filename << std::endl;

  return 0;
}


// *****************************************************
// *****************************************************
// *****************************************************
// *****************************************************

//**************************************************************************
//! memcpy implemenation where the semantics are known; copies left to right.
/*!
 * memcpy is not safe when copying to the same block of memory.
 * The implemenation is fast, but may copy from either end.
 * This can be made faster if needed. 
 * 
 * Copies from left on each side moving towards the right.
 * This allows copying data "down" (to the left) without
 * over-writing. The built-in memory move has well defined 
 * semantics. 
 * 
 * \param [out] dest Copy to here
 *
 * \param [in] src Copy from here
 * 
 * \param [in] n - Number of bytes to copy
 * 
 ***************************************************************************///
void memcpy_ltor(u_char *dest, u_char *src, uint32_t n) {
  // I know the direction and I will let 
  // the compiler optimize it.
  // I will revisit if needed.
  for (uint32_t i=0; i<n; ++i) {
    dest[i] = src[i];
  }
}

bool strip_mpls_method(struct pcap_pkthdr& pkt_header, u_char *pkt_data) {

  uint32_t mpls_start = sizeof(struct ether_header);
  uint32_t mpls_len = 0;
  int new_ethertype = 0;

  // Assume this is an MPLS packet.
  // 20 for the label
  // 03 experimental bits
  // 01 bit (bottom of stack flag)
  // 08 bits Time To Live

  // Bottom of stack flag with a value of 0 means
  // that there is another MPLS flag.
  uint8_t bos;
  do {
    std::cout << "MPLS Layer." << std::endl;
    // Deal with truncated MPLS.
    if (pkt_header.len < (mpls_start + mpls_len + 4)) {
      std::cout << "Truncated Packet(mpls) at length " << (mpls_len + 4) << std::endl;
      return false;
    }
    //uint32_t label = (pkt_data[mpls_start + mpls_len + 2] >> 4) + pkt_data[mpls_start + mpls_len + 1] * 16 + pkt_data[mpls_start + mpls_len] * 4096;
    //std::cout << "MPLS Layer label = " << std::hex << label << std::dec << " or " << label << std::endl;
    bos = pkt_data[mpls_start + mpls_len + 2] & 0x01;
    mpls_len += 4;
  } while (bos == 0);

  if (pkt_header.len < (mpls_start + mpls_len)) {
    std::cout << "Truncated Packet(mpls) at length " << mpls_len << std::endl;
    return false;
  }

  // 'Guess' the next protocol. This can result in false positives.
  uint8_t ip_ver = pkt_data[mpls_start + mpls_len] >> 4;
  switch (ip_ver) {
    case 0x04:
      new_ethertype = 0x0800; break;
    case 0x06:
      new_ethertype = 0x86DD; break;
    default:
      // TODO: handle this
      std::cout << "Warning, returning ethernet type 0 from MPLS" << std::endl;
      new_ethertype = 0;
  }

  // now shift things around
  if (new_ethertype != 0) {

      // Leave the first portion up to mpls_start
      memcpy_ltor( pkt_data + mpls_start, pkt_data + mpls_start + mpls_len,  pkt_header.len - mpls_start - mpls_len);
      struct ether_header* ethernet_header = (struct ether_header*)pkt_data;
      ethernet_header->ether_type = htons(new_ethertype);
      pkt_header.len -= mpls_len;
      pkt_header.caplen = pkt_header.len;
      return true;
  }

  return false;
}

bool strip_macsec_method(struct pcap_pkthdr& pkt_header, u_char *pkt_data) {

  // Reduce packet header lenght by 32
  // Remove 16 bytes from the macsec header and 16 bytes from the ICV at the end.
  if (pkt_header.len < 32) {
    std::cout << "Error stripping macsec, packet header length is less than 32" << std::endl;
    return false;
  }
  pkt_header.len -= 32;
  pkt_header.caplen -= 32;

  // used to skip the first 28 bytes of a frame to check if there is vlan following it
  const int macsec_offset_etype = 28;

  int macsec_etype_int = ntohs(*(uint16_t *)(pkt_data + macsec_offset_etype));
  // a VLAN type in MACSEC.
  if(macsec_etype_int == 0x8000) {
    pkt_header.len -= 4;
    pkt_header.caplen -= 4;
    memcpy_ltor(pkt_data + 12, pkt_data + macsec_offset_etype + 4, pkt_header.len - 12);
  } else {
    // First 12 bytes (Mac source and destination) remain the same. 
    memcpy_ltor(pkt_data + 12, pkt_data + macsec_offset_etype, pkt_header.len - 12);
  }

  return true;
}

bool strip_vlan_method(struct pcap_pkthdr& pkt_header, u_char *pkt_data) {
  // TODO: ????
  // Do I need to recalculate the FCS?
  // Just blindly remove four bytes!
  // Leave the header of size sizeof(struct ether_header) = 14
  if (pkt_header.len > 16) {
    pkt_header.len -= 4;
    pkt_header.caplen -= 4;
    // There is something to copy.
    // I over-write the last two bytes of the ethernet header
    // which is the type id.
    // So remove the 4 bytes and do not copy the 12 destination
    // and source MAC addresses.
    // Already changed the header length however. 
    memcpy_ltor( pkt_data+sizeof(struct ether_header)-2, pkt_data+sizeof(struct ether_header)+2, pkt_header.len - 12);
  } else {
    std::cout << "Packet header lenght is not long enough to strip a VLAN tag." << std::endl;
    return false;
  }
  return true;
}

int strip_stuff(const std::string& pcap_filename, std::string output_directory, std::string extra_heuristic_name, std::atomic_bool* abort_requested, int strip_mpls_u, int strip_mpls_m, int strip_qq, int strip_ct, int strip_macsec) {

  std::string output_fname = getHeuristicFileName(pcap_filename, Anomaly_Type, output_directory, extra_heuristic_name);
  if (isPathExist(output_fname, true, false, false, false)) {
    std::cout << "Output file will be over-written: " << output_fname << std::endl;
  }

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
  struct pcap_pkthdr pkt_header_copy;

  const u_char *pkt_data;
  uint32_t data_copy_len = 1024 * 1024 * 4;
  u_char *pkt_data_copy = new u_char[data_copy_len + 1];

  struct ether_header *ethernet_header;

  // Accepts the return code from libpcap.
  int res=1;

  // packet counter. Increments after the packet is processed.
  int it=1;

  // Number of specific types / items found.
  int num_mpls_u=0, num_mpls_m=0, num_qq=0, num_ct=0, num_macsec=0;

  // Open the PCAP file!
  pcap_file=pcap_open_offline(pcap_filename.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_filename.c_str());
    pcap_close(pcap_file);
    return -1;
  }

  // Open the dump file
  dumpfile = pcap_dump_open(pcap_file, output_fname.c_str());

  if(dumpfile==NULL)
  {
    pcap_close(pcap_file);
    fprintf(stderr,"\nError opening output file\n");
    return -1;
  }

  bool packet_modified;

  // Iterate over every packet in the file and print the MAC addresses
  while(!done && (abort_requested == nullptr || !abort_requested->load()) ){
    // pkt_header contains three fields:
    // struct timeval ts (time stamp) with tv_sec and tv_usec for seconds and micro-seconds I guess.
    // uint32_t caplen (length of portion present)
    // uint32_t len (length of this packet off wire) - 
    //
    // The len field is reading fine when referenced directly without converting 
    // from network byte order (big-endian) to host endian (little-endian)
    // using ntohl().
    //
    // pkt_data points to the data.
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);

    if(res == PCAP_ERROR_BREAK){
      fprintf(stderr, "No more packets in savefile. Iteration %d\n", it);
      break;
    }
    if(res != 1){
      fprintf(stderr, "Error reading packet. Iteration %d\n", it);
      continue;
    }

    // Works with a copy of the data rather than 
    // with the data since I do not know if I 
    // can safely modify the data in place.
    // Probably not.
    // Verify that the buffer is sufficiently large.
    if (pkt_header->len > data_copy_len) {
      delete[] pkt_data_copy;
      data_copy_len = pkt_header->len * 2;
      pkt_data_copy = new u_char[data_copy_len + 1];
    }

    // Copy the packet header
    memcpy( &pkt_header_copy, pkt_header, sizeof(struct pcap_pkthdr) );
    memcpy( pkt_data_copy, pkt_data, pkt_header_copy.len);

    // The packet data begins with the Ethernet header, so if we cast to that:
    // uint8_t   ether_dhost [6]  MAC address
    // uint8_t   ether_shost [6]  MAC address
    // uint16_t  etherType        Ethernet type
    ethernet_header = (struct ether_header*)pkt_data_copy;
    int ether_type_int = ntohs(ethernet_header->ether_type);

    
    do {
      packet_modified = false;
      if (strip_mpls_u && ether_type_int == 0x8847) {
        packet_modified = strip_macsec_method(pkt_header_copy, pkt_data_copy);
        if (packet_modified) {
          ++num_mpls_u;
          ether_type_int = ntohs(ethernet_header->ether_type);
        }
      }
      if (strip_mpls_m && ether_type_int == 0x8848) {
        // Do Something
        ++num_mpls_m;
      }
      if (strip_qq && ether_type_int == 0x8A88) {
        packet_modified = strip_vlan_method(pkt_header_copy, pkt_data_copy);
        if (packet_modified) {
          ++num_qq;
          ether_type_int = ntohs(ethernet_header->ether_type);
        }
      }
      if (strip_ct && ether_type_int == 0x8100) {
        packet_modified = strip_vlan_method(pkt_header_copy, pkt_data_copy);
        if (packet_modified) {
          ++num_ct;
          ether_type_int = ntohs(ethernet_header->ether_type);
        }
      }
      if (strip_macsec && ether_type_int == 0x88E5) {
        packet_modified = strip_macsec_method(pkt_header_copy, pkt_data_copy);
        if (packet_modified) {
          ++num_macsec;
          ++num_ct;
          ether_type_int = ntohs(ethernet_header->ether_type);
        }
      }
    } while (packet_modified);

    pcap_dump( (u_char *)dumpfile, &pkt_header_copy, pkt_data_copy);

    // Done with all processing for this packet.
    ++it;
  }

  delete[] pkt_data_copy;
  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cout << " Examined "<< (it - 1) <<" packets in file " << pcap_filename << std::endl;
  if (num_mpls_u > 0)
    std::cout << "Removed " << num_mpls_u << " MPLS Unicast (8847) packet headers" << std::endl;
  if (num_mpls_m > 0)
    std::cout << "Removed " << num_mpls_m << " MPLS Multicast (8848) packet headers" << std::endl;
  if (num_qq > 0)
    std::cout << "Removed " << num_qq << " Q-in-Q / Service Tag (0x8A88) packet headers" << std::endl;
  if (num_ct > 0)
    std::cout << "Removed " << num_ct << " Customer Tag (0x8100) packet headers" << std::endl;
  if (num_macsec > 0)
    std::cout << "Removed " << num_macsec << " Macsec (0x88E5) packet headers" << std::endl;

  return 0;
}

void usage(){
  std::cout << "Usage" << std::endl;
  std::cout << "  --help    Print usage instructions." << std::endl;
  std::cout << "  --pcap <NAME> Full path to the PCAP file to read." << std::endl;
  std::cout << "  --directory <NAME> Output directory where files are stored." << std::endl;
  std::cout << "            defaults to the directory containing the PCAP ile." << std::endl;
  std::cout << "  --name <NAME> By default, file.pcap generates file.anomaly.pcap" << std::endl;
  std::cout << "            use --name bob to generate file.bob.pcap instead.  " << std::endl;
  std::cout << "  --macsec    Strip MACSEC." << std::endl;
  std::cout << "  --88E5      Strip MACSEC" << std::endl;
  std::cout << "  --8100      Strip VLAN (Customer Tag)." << std::endl;
  std::cout << "  --8A88      Strip VLAN (Q-in-Q)." << std::endl;
  std::cout << "  --vlan (-v) Strip VLAN (Customer Tag and Q-in-Q)." << std::endl;
  std::cout << "  --8847      Strip MPLS (MPLS Unicast)." << std::endl;
  std::cout << "  --8848      Strip MPLS (MPLS Multicast)." << std::endl;
  std::cout << "  --mpls (-m) Strip MPLS (MPLS Multicast and Unicast)." << std::endl;
  std::cout << "  " << std::endl;
  std::cout << std::endl;
  std::cout << "  -? Shortened version of --help" << std::endl;
  std::cout << "  -p Shortened version of --pcap" << std::endl;
  std::cout << "  -d Shortened version of --directory" << std::endl;
  std::cout << "  -n Shortened version of --name" << std::endl;
  std::cout << std::endl;
  std::cout << "All filenames are generated, you cannot choose them." << std::endl;
  std::cout << "Existing destination files are over-written." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "You can turn off some items by prepending the word no:" << std::endl;
  std::cout << "nomacsec, no88E5, no8100, no8A88, no8847, and no8848" << std::endl;
  std::cout << "" << std::endl;
  std::cout << std::endl;
}

int main(int argc, char **argv) {
  std::string pcap_filename = "";
  pcap_filename = "/andrew0/home/andy/Devsrc/Battelle/GreenHornet/git_stuff/toolbox_git/toolbox/wireshark/mpls/EoMPLS.pcap";
  std::string output_directory = "out";
  std::string extra_heuristic_name = "no_mpls";
  std::atomic_bool* abort_requested = nullptr;
  std::string extra_name = "stripped";
  bool remove_mac_sec = false;

  int c;
  while (1) {
    static struct option long_options[] =
    {
      /* These options set a flag. */
      {"8100",     no_argument,       &strip_8100, 1},  // VLAN Tagged Fram (Customer Tag)
      {"8A88",     no_argument,       &strip_8A88, 1},  // VLAN Tagged Fram (Q-in-Q)
      {"8847",     no_argument,       &strip_8847, 1},  // MPLS Unicast
      {"8848",     no_argument,       &strip_8848, 1},  // MPLS Multicast
      {"88E5",     no_argument,       &strip_88E5, 1},  // MacSec
      {"macsec",   no_argument,       &strip_88E5, 1},  // MacSec

      {"no8100",   no_argument,       &strip_8100, 0},
      {"no8A88",   no_argument,       &strip_8A88, 0},
      {"no8847",   no_argument,       &strip_8847, 0},
      {"no8848",   no_argument,       &strip_8848, 0},
      {"nomacsec", no_argument,       &strip_88E5, 0},
      {"no88E5",   no_argument,       &strip_88E5, 0},
      
      /* Can I set an int directly */
      /* These options don’t set a flag. We distinguish them by their indices. */
      {"help",      no_argument,       0, '?'},
      {"pcap",      required_argument, 0, 'p'},
      {"directory", required_argument, 0, 'd'},
      {"name",      required_argument, 0, 'n'},
      {"mpls",      no_argument,       0, 'm'},
      {"vlan",      no_argument,       0, 'v'},
      {0, 0, 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "?p:d:nmv", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 0:
        // If this option set a flag, do nothing else now.
        // Used an option such as --verbose, which is a flag.
        if (long_options[option_index].flag != 0) {
          break;
        }
        // I do not think that we will ever get here.
        if (optarg)
          std::cout << "option " << long_options[option_index].name << " has option " << optarg << std::endl;
        else
          std::cout << "option " << long_options[option_index].name << " has option nullptr" << std::endl;
        break;

      case 'h':
        usage();
        return -1;
        break;

      case 'p':
        pcap_filename = optarg ? optarg : "";
        break;

      case 'd':
        output_directory = optarg ? optarg : "";
        break;

      case 'm':
        strip_8848 = strip_8847 = 1;
        break;

      case 'n':
        extra_name = optarg ? optarg : "";
        break;

      case 'v':
        strip_8100 = strip_8A88 = 1;
        break;

      case '?':
        usage();
        exit(1);
        break;

      default:
        printf("Aborting\n");
        exit(1);
      }
  }


  if(pcap_filename.empty()){
    std::cout << "PCAP file must be specified." << std::endl;
    usage();
    exit(1);
  }

std::cout << "Out of the loop" << std::endl;

  if (optind < argc)
  {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    putchar ('\n');
  }
std::cout << "Check for unknown options." << std::endl;
  // If any argument was given that isn't a known option, print the usage and exit
  for (int index = optind; index < argc; index++){
    usage();
    exit(1);
  }


  if (!isPathExist(pcap_filename, true, false, false, false)) {
    std::cout << "PCAP file does not exist: " << pcap_filename << std::endl;
    return -1;
  }
  if (!isPathExist(pcap_filename, true, false, true, false)) {
    std::cout << "Do not have read permission on PCAP file: " << pcap_filename << std::endl;
    std::cout << "This may fail to read the PCAP file" << std::endl;
    //return -1;
  }

  if (output_directory.empty())
    output_directory = getDirectoryFromFilename(pcap_filename);

  if (abort_requested != nullptr) 
    std::cout << "What???" << std::endl;
  if (remove_mac_sec)
    std::cout << "Remove mac_sec" << std::endl;

  //process_mpls(pcap_filename, output_directory, extra_heuristic_name, abort_requested);
}