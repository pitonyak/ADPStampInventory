
#include <iostream>
#include <iomanip>
#include <net/ethernet.h> // struct ether_header
#include <netinet/ip.h>   // struct ip
#include <netinet/ip6.h>  // struct ip6_hdr

#include "process_pcap.h"
#include "pcap.h"
#include "utilities.h"

int write_ip_and_mac_from_pcap(MacAddresses& mac_addresses, IpAddresses& ip_addresses, const std::string& pcap_filename, const std::string& out_mac_fname, const std::string& out_ip_fname, std::atomic_bool* abort_requested) {
  mac_addresses.clear();
  ip_addresses.clear();

  pcap_t *pcap_file;
  char *pcap_errbuf = nullptr;
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;

  const struct ether_header *ether;
  u_int done=0;
  int res=1, it=0;

  pcap_file=pcap_open_offline(pcap_filename.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    std::cerr << "PCAP file " << pcap_filename << "is not an Ethernet capture\n" << std::endl;
  }

  // Iterate over every packet in the file and print the MAC addresses
  while(!done && (abort_requested == nullptr || !abort_requested->load()) ){
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

      auto source_ip_address = ipHeader->ip_src;
      auto dest_ip_address = ipHeader->ip_dst;

      bool is_any_address = (source_ip_address.s_addr == INADDR_ANY || dest_ip_address.s_addr == INADDR_ANY);
      bool is_broadcast_address = (source_ip_address.s_addr == INADDR_BROADCAST || dest_ip_address.s_addr == INADDR_BROADCAST);
      bool is_multi = (IpAddresses::is_multicast_address(source_ip_address.s_addr) || IpAddresses::is_multicast_address(dest_ip_address.s_addr)) ;

      if (is_broadcast_address) {
        std::cout << "Skipping Packet containing a broadcast address: 255.255.255.255" << std::endl;
        continue;
      }
      else if (is_any_address) {
        std::cout << "Skipping Packet containing a non-routable target with address: 0.0.0.0" << std::endl;
        continue;
      }
      else if (is_multi) {
        std::cout << "Skipping Packet containing a multicast address with ip address range: 224.x.x.x.-239.x.x.x" << std::endl;
        continue;
      }

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

  // Write the unique MACs to the output file (stdout by default), and then reset std::cout to the default stdout
  if (out_mac_fname.length() > 0) {
    mac_addresses.write_file(out_mac_fname);
  }

  // Write the unique IPs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_ip_fname.length() > 0) {
    ip_addresses.write_file(out_ip_fname);
  }

  std::cerr << "Created IP and MAC files from "<< it <<" packets"<<std::endl;
  return 0;
}

void read_create_mac_ip_files(MacAddresses& mac_addresses, IpAddresses& ip_addresses, const std::string& pcap_filename, std::string output_directory, std::string extra_heuristic_name, std::atomic_bool* abort_requested) {
  std::string mac_fname = getHeuristicFileName(pcap_filename, MAC_Type, output_directory, extra_heuristic_name);
  std::string ip_fname = getHeuristicFileName(pcap_filename, IP_Type, output_directory, extra_heuristic_name);
  
  bool create_mac_ip_files = !isPathExist(mac_fname, true, false, false, false) || !isPathExist(ip_fname, true, false, false, false);
  if (create_mac_ip_files) {
    std::string path = getDirectoryFromFilename(mac_fname);
    if (!isPathExist(path, false, true, true, true)) {
      std::cout << "Cannot read/write to directory where the MAC file will be created: " << path << std::endl;
      std::cout << "This may fail to create the MAC file " << mac_fname << std::endl;
    }
    std::cout << " creating files " << mac_fname << " and " << ip_fname << std::endl;
    write_ip_and_mac_from_pcap(mac_addresses, ip_addresses, pcap_filename, mac_fname, ip_fname, abort_requested);
  } else {
    // Check for read permission, we already know the files exist.
    if (!isPathExist(mac_fname, true, false, true, false) || !isPathExist(ip_fname, true, false, true, false)) {
      std::cout << "ERROR: Cannot read " << mac_fname << " or " << ip_fname << std::endl;
      std::cout << "This may fail to create the MAC and IP files" << std::endl;
    }
    std::cout << " reading files " << mac_fname << " and " << ip_fname << std::endl;
    mac_addresses.read_file(mac_fname);
    ip_addresses.read_file(ip_fname);
  }
}
