#include <chrono>
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
#include <stdio.h>
#include <stdlib.h>

#include "heuristicthread.h"
#include "ahocorasickbinary.h"
#include "crc32_x.h"
#include "csvwriter.h"
#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "pcap.h"
#include "utilities.h"



HeuristicThread::HeuristicThread(int thread_id, std::string output_directory, bool generate_csv, bool verbose)
	: m_thread_id(thread_id), m_output_directory(output_directory), 
      m_generate_csv(generate_csv), m_verbose(verbose)
{
    m_working.store(false);
    m_running.store(false);
    m_abort_requested.store(false);
    m_extra_heuristic_name = "anomaly";

    m_ip_types.readProtocols("ip_protocols.txt", false, 10);
    m_ip_types.readProtocolPorts("ip_protocol_ports.txt");

    m_ethernet_types.read("eth_types.txt");
    m_dest_mac_to_ignore.read_file("dest_mac_to_ignore");

}

HeuristicThread::~HeuristicThread()
{
    abortAndJoin();
}

void HeuristicThread::setOutputDirectory(const std::string& output_directory) { 
    m_output_directory = output_directory;
    if (!m_output_directory.empty() && !hasEnding(m_output_directory, "/", false)) {
        m_output_directory.append("/");
    }
}

void HeuristicThread::abortAndJoin()
{
    m_abort_requested.store(true);
    if(m_thread.joinable())
    {
        m_thread.join();
    }
}

bool HeuristicThread::run()
{
    if (m_running.load()) {
        std::cout << "Running called on thread " << m_thread_id << " while it is already running." << std::endl;
        return false;
    }
    try 
    {
        m_thread = std::thread(&HeuristicThread::runFunc, this);
    }
    catch(...) 
    {
        std::cout << "Exception Starting thread " << m_thread_id << std::endl;
        return false;
    }

    std::cout << "Started thread " << m_thread_id << std::endl;
    return true;
}


void HeuristicThread::runFunc() 
{
    m_running.store(true);

    // We now check against abort criteria
    while(!m_abort_requested.load())
    {
        try
        {
            // Do something...
            // Need to wait for work to do!
            std::string next_pcap_filename;
            try {
                std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
                next_pcap_filename = m_pcap_name;
                m_pcap_name = "";
            }
            catch(...) 
            {
                // Make sure that nothing leaves the thread for now...
            }
            if (!next_pcap_filename.empty()) {
                doWork(next_pcap_filename);
            } else {
                // Sleep for a bit!
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        catch(std::runtime_error& e) 
        {
            // Some more specific
        }
        catch(...) 
        {
            // Make sure that nothing leaves the thread for now...
        }
    }

    m_running.store(false);
    m_working.store(false);
    m_abort_requested.store(true);
}

void HeuristicThread::setNextPcap(std::string pcap_filename) {
    std::lock_guard guard(m_pcap_name_mutex);
    m_pcap_name = pcap_filename;
}

void HeuristicThread::doWork(std::string pcap_filename) {
    if (m_working.load()) {
        std::cout << "doWork called on an already working thread: " << m_thread_id << " for file " << pcap_filename << std::endl;
        return;
    }
    m_working.store(true);
    // Do what needs to be done here!
    // ??????
    try {

        read_create_mac_ip_files(pcap_filename);
        create_heuristic_anomaly_csv(m_ethernet_types, m_ip_types, pcap_filename, m_verbose, m_generate_csv);
    }
    catch(std::runtime_error& e) 
    {
        // Some more specific
    }
    catch(...) 
    {
        // Make sure that nothing leaves the thread for now...
    }
    m_working.store(false);
}

void HeuristicThread::read_create_mac_ip_files(const std::string& pcap_filename) {
  std::string mac_fname = getHeuristicFileName(pcap_filename, MAC_Type, m_output_directory, m_extra_heuristic_name);
  std::string ip_fname = getHeuristicFileName(pcap_filename, IP_Type, m_output_directory, m_extra_heuristic_name);
  
  bool create_mac_ip_files = !isPathExist(mac_fname, true, false, false, false) || !isPathExist(ip_fname, true, false, false, false);

  if (create_mac_ip_files) {
    std::string path = getDirectoryFromFilename(mac_fname);
    if (!isPathExist(path, false, true, true, true)) {
      std::cout << "Cannot read/write to directory where the MAC file will be created: " << path << std::endl;
      std::cout << "This may fail to create the MAC file " << mac_fname << std::endl;
    }
    std::cout << " creating files " << mac_fname << " and " << ip_fname << std::endl;
    write_ip_and_mac_from_pcap(pcap_filename, mac_fname, ip_fname);
  } else {
    // Check for read permission, we already know the files exist.
    if (!isPathExist(mac_fname, true, false, true, false) || !isPathExist(ip_fname, true, false, true, false)) {
      std::cout << "ERROR: Cannot read " << mac_fname << " or " << ip_fname << std::endl;
      std::cout << "This may fail to create the MAC and IP files" << std::endl;
    }
    std::cout << " reading files " << mac_fname << " and " << ip_fname << std::endl;
    m_mac_addresses.read_file(mac_fname);
    m_ip_addresses.read_file(ip_fname);
  }
}

int HeuristicThread::write_ip_and_mac_from_pcap(const std::string& pcap_filename, const std::string& out_mac_fname, const std::string& out_ip_fname) {
  m_mac_addresses.clear();
  m_ip_addresses.clear();

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
    m_mac_addresses.addMacAddress(ether->ether_shost);
    m_mac_addresses.addMacAddress(ether->ether_dhost);

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
      m_ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip_src), true, false);
      m_ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip_dst), true, false);
    }
    else if (ntohs(ether->ether_type) == ETHERTYPE_IPV6) {
      const struct ip6_hdr* ipHeader;
      ipHeader = (struct ip6_hdr*)(pkt_data + sizeof(struct ether_header));
      m_ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip6_src), false, false);
      m_ip_addresses.addIpAddress((uint8_t*)&(ipHeader->ip6_dst), false, false);
    }

    it++;
  }

  pcap_close(pcap_file);

  // Write the unique MACs to the output file (stdout by default), and then reset std::cout to the default stdout
  if (out_mac_fname.length() > 0) {
    m_mac_addresses.write_file(out_mac_fname);
  }

  // Write the unique IPs to the output file (stdout by default), and then reset std::cout to the default stdout
  if(out_ip_fname.length() > 0) {
    m_ip_addresses.write_file(out_ip_fname);
  }

  std::cerr << "Created IP and MAC files from "<< it <<" packets"<<std::endl;
  return 0;
}

int HeuristicThread::create_heuristic_anomaly_csv(const EthernetTypes& ethernet_types, const IPTypes& ip_types, const std::string& pcap_filename, bool verbose, bool generateCSV) {
  std::string csv_fname = getHeuristicFileName(pcap_filename, CSV_Type, m_output_directory, m_extra_heuristic_name);
  std::string anomaly_fname = getHeuristicFileName(pcap_filename, Anomaly_Type, m_output_directory, m_extra_heuristic_name);
  read_create_mac_ip_files(pcap_filename);
  if (isPathExist(anomaly_fname, true, false, false, false)) {
    std::cout << "Anomaly file will be over-written: " << anomaly_fname << std::endl;
  }
  // Time 2.898seconds becomes 11m19.323seconds if generateCSV is true for one example file.
  pcap_t *pcap_file;
  pcap_dumper_t *dumpfile;
  bool done = false;

  // Aho-Corasick is significantly faster that forward or backward search.
  AhoCorasickBinary search_ipv4;
  AhoCorasickBinary search_ipv6;
  AhoCorasickBinary search_macs;

  std::unique_ptr<std::vector<uint8_t *>> words_ipv4 = m_ip_addresses.toVector(true);
  std::unique_ptr<std::vector<uint8_t *>> words_ipv6 = m_ip_addresses.toVector(false);
  std::unique_ptr<std::vector<uint8_t *>> words_macs = m_mac_addresses.toVector();
  std::unique_ptr<std::vector<int>> lengths_ipv4 = getConstWordLengthVector( 4, m_ip_addresses.m_unique_ipv4.size());
  std::unique_ptr<std::vector<int>> lengths_ipv6 = getConstWordLengthVector(16, m_ip_addresses.m_unique_ipv6.size());
  std::unique_ptr<std::vector<int>> lengths_macs = getConstWordLengthVector( 6, m_mac_addresses.m_unique_macs.size());

  std::cout << "Initializing ipv4 search" << std::endl;
  search_ipv4.buildMatchingMachine(*words_ipv4.get(), *lengths_ipv4.get());
  std::cout << "Initializing ipv6 search" << std::endl;
  search_ipv6.buildMatchingMachine(*words_ipv6.get(), *lengths_ipv6.get());
  std::cout << "Initializing macs search" << std::endl;
  search_macs.buildMatchingMachine(*words_macs.get(), *lengths_macs.get());

  // We do not need to keep or remember the words because we do not track what was found.
  // If we wanted to remember that, then we would need to keep the words vectors.
  // So, release the memory using reset. If you intend to use the words, then
  // do not do this.
  words_ipv4.reset();
  words_ipv6.reset();
  words_macs.reset();

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
  pcap_file=pcap_open_offline(pcap_filename.c_str(), pcap_errbuf);

  // Ensure that the pcap file only has Ethernet packets
  if(pcap_datalink(pcap_file) != DLT_EN10MB){
    fprintf(stderr, "PCAP file %s is not an Ethernet capture\n", pcap_filename.c_str());
    pcap_close(pcap_file);
    return -1;
  }

  // Open the dump file
  dumpfile = pcap_dump_open(pcap_file, anomaly_fname.c_str());

  if(dumpfile==NULL)
  {
    pcap_close(pcap_file);
    fprintf(stderr,"\nError opening output file\n");
    return -1;
  }

  std::vector<std::string> heading;
  if (generateCSV) {
    heading.push_back("Frame Number");
    heading.push_back("Source IP");
    heading.push_back("Destination IP");
    heading.push_back("Unique IP Matches");
    heading.push_back("Unique MAC Matches");
    heading.push_back("Total IP Matches");
    heading.push_back("Total MAC Matches");
  }

  std::unique_ptr<CSVWriter> csv(generateCSV ? new CSVWriter(csv_fname) : nullptr);
  // CSV Numbering now starts with record 1 rather than 0, which
  // matches the frame numbering used in Wireshark, which is
  // one based rather than zero-based.
  int output_row = 1;
  bool has_header = false;
  bool may_have_dup = false;

  const int ether_header_size = sizeof(struct ether_header); // 14
  const int ip_header_size = sizeof(struct ip); // 20
  //const int tcphdr_size = sizeof(struct tcphdr); // 20
  //const int udphdr_size = sizeof(struct udphdr); // 8
  const int offset_to_data_ipv4 = ether_header_size + ip_header_size; // 34
  //const int offset_to_data_tcpv4 = offset_to_data_ipv4 + tcphdr_size;
  //const int offset_to_data_udpv4 = offset_to_data_ipv4 + udphdr_size;

  //int offset_to_search_data;

  [[maybe_unused]] const int offset_to_data_ipv6 = ether_header_size + sizeof(struct ip6_hdr);

  int num_ip_found_unique = 0;
  int num_mac_found_unique = 0;
  int num_ip_found_total = 0;
  int num_mac_found_total = 0;

  // Iterate over every packet in the file and print the MAC addresses
  while(!done){
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
    res=pcap_next_ex(pcap_file, &pkt_header, &pkt_data);

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

    // Some protocols do NOT have an associated ether type;
    // for example, Cisco Discovery Protocol and VLAN Trunking Protocol.
    // This provides a means of ignoring certain broadcast messages.
    //
    if (m_dest_mac_to_ignore.hasAddress(ether->ether_dhost)) {
      if (verbose) std::cout << it << " Destination MAC is in the ignore list." << std::endl;
      ++it;
      continue;
    }

    int ether_type_int = ntohs(ether->ether_type);

    if (!ethernet_types.isValid(ether_type_int)) {
      if (verbose)
        std::cout << it << " has unexpected ether type " << std::hex << ether_type_int << std::dec << std::endl;
      
      // Check for valid Frame Check Sequence (FCS) as per the flow diagram.
      // We ignore the FCS for now because we do not know if it will be available.
      // As of 09/29/2022 it is assumed that we will not check for this.

      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      it++;
      ++output_row;
      continue;
    }

    // Check for type IP (0x800)
    // This type is skipped for some types, but not others.
    // Set the eth_types.txt file so that it does NOT indicate that
    // this type has copies of the IP or MAC and rely on this check instead.
    // The header follows the ether_header
    int tcp_destination_port = -1;
    int tcp_source_port = -1;
    int ip_p = -1;
    std::string proto = "Unknown";
    std::string ip_str_source;
    std::string ip_str_dest;
    if (ether_type_int == ETHERTYPE_IP) {

      const struct ip* ipHeader;
      ipHeader = (struct ip*)(pkt_data + ether_header_size);
      //offset_to_search_data = offset_to_data_ipv4;

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
      ip_p = static_cast<int>(ipHeader->ip_p);
      if (ip_p == static_cast<int>(IPPROTO_TCP)) {

        // IPPROTO_TCP = 6
        // struct tcphdr with source and destination ports
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        // 
        // Port 161 could be in source or destination. 
        //
        const struct tcphdr* tcp_header = (const struct tcphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = ntohs(tcp_header->th_dport);
        tcp_source_port = ntohs(tcp_header->th_sport);
        proto = "TCP";
        //offset_to_search_data = offset_to_data_tcpv4;

      } else if (ip_p == static_cast<int>(IPPROTO_UDP)) {

        // IPPROTO_UDP = 17
        // struct udphdr
        // uint16_t th_sport;      source port (network byte order)
        // uint16_t th_dport;      destination port (network byte order)
        //
        const struct udphdr* udp_header = (const struct udphdr*)(pkt_data + offset_to_data_ipv4);
        tcp_destination_port = ntohs(udp_header->uh_dport);
        tcp_source_port = ntohs(udp_header->uh_sport);
        proto = "UDP";
        //offset_to_search_data = offset_to_data_udpv4;
      }

      // If we get here then we can search for duplicates.
      // Look AFTER the IP header for dupliate IP.

      // Search to see if a MAC is repeated.
      may_have_dup = ip_types.isDupMAC(ip_p, tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupMAC(ip_p, tcp_source_port));
      //std::cout << "May MAC Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        //
        // The 12 has us search from immediately AFTER the MAC addresses in the
        // Ether header. Might be faster to search AFTER the data.
        //
        uint32_t search_len = pkt_header->len - 12;
        const uint8_t* data_loc = (pkt_data + 12);
        //uint32_t search_len = pkt_header->len - offset_to_search_data;
        //const uint8_t* data_loc = (pkt_data + offset_to_search_data);

        if (generateCSV) {
          std::map<int, std::set<int> > matches = search_macs.findAllMatches(data_loc, search_len);
          num_mac_found_unique += matches.size();
          num_mac_found_total += search_macs.countMatches(matches);
        } else if (search_macs.findFirstMatch(data_loc, search_len) >= 0) {
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
          it++;
          ++output_row;
          continue;
        }
      }

      may_have_dup = ip_types.isDupIP(ip_p, tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupIP(ip_p, tcp_source_port));
      //std::cout << "May  IP Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        //
        // This will search the TCP and UDP headers.
        // Should we change to ONLY search the data?
        //
        uint32_t search_len = pkt_header->len - offset_to_data_ipv4;
        const uint8_t* data_loc = pkt_data + offset_to_data_ipv4;
        //uint32_t search_len = pkt_header->len - offset_to_search_data;
        //const uint8_t* data_loc = (pkt_data + offset_to_search_data);

        if (generateCSV) {
          std::map<int, std::set<int> > matches_4 = search_ipv4.findAllMatches(data_loc, search_len);
          std::map<int, std::set<int> > matches_6 = search_ipv6.findAllMatches(data_loc, search_len);
          num_ip_found_unique += matches_4.size();
          num_ip_found_unique += matches_6.size();
          num_ip_found_total += search_ipv4.countMatches(matches_4);
          num_ip_found_total += search_ipv6.countMatches(matches_6);
        } else if (
          (search_ipv4.findFirstMatch(data_loc, search_len) >= 0) ||
          (search_ipv6.findFirstMatch(data_loc, search_len) >= 0)) {
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
          it++;
          ++output_row;
          continue;
        }
      }

      // This can only be true if we are generating a CSV because if not, 
      // would have already used continue.
      if (num_mac_found_unique > 0 || num_ip_found_unique > 0) {
        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);

        if (m_verbose)
          std::cout << it << " DUP MAC: " << num_mac_found_unique << " IP:" << num_ip_found_unique << " with Ethertype IPv4 protocol = " << proto << " (" << ip_p << ")" << " ports: " << tcp_source_port << " / " << tcp_destination_port << std::endl;

        // We are here which means that we are generating a CSV.
        if (!has_header) {
          *csv << heading;
          csv->endRow();
          has_header = true;
        }
        *csv << output_row
            << IpAddresses::ip_to_str((uint8_t*)&(ipHeader->ip_src), true)
            << IpAddresses::ip_to_str((uint8_t*)&(ipHeader->ip_dst), true)
            << num_ip_found_unique
            << num_mac_found_unique
            << num_ip_found_total
            << num_mac_found_total;
        csv->endRow();
        num_ip_found_unique = 0;
        num_mac_found_unique = 0;
        num_ip_found_total = 0;
        num_mac_found_total = 0;

        it++;
        ++output_row;
        continue;
      }

      // We are done with this ipv4 packet. No need to search more.
      // The packet does not contain duplicate data!
      if (m_verbose)
        std::cout << it << " Skipping packet with Ethertype IPv4 protocol = " << proto << " (" << ip_p << ")" << " ports: " << tcp_source_port << " / " << tcp_destination_port << std::endl;
      ++it;
      continue;
    }

    // Done processing IPv4. We are here, so this is NOT IPv4 so check for IPv6.

    // In case I need to remember how to deal with an IPv6 header.
    // I expect this to work, but, it is not tested code since I
    // have no IPv6 fiels against which to test.
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
        tcp_destination_port = ntohs(tcp_header->th_dport);
        tcp_source_port = ntohs(tcp_header->th_sport);
        proto = "TCP";

      } else if (static_cast<int>(next_header) == static_cast<int>(IPPROTO_UDP)) {

        // IPPROTO_UDP = 17
        // struct udphdr
        // uint16_t th_sport;      source port
        // uint16_t th_dport;      destination port
        //
        const struct udphdr* udp_header = (const struct udphdr*)(pkt_data + offset_to_data_ipv6);
        final_data_offset += sizeof(struct udphdr);
        tcp_destination_port = ntohs(udp_header->uh_dport);
        tcp_source_port = ntohs(udp_header->uh_sport);
        proto = "UDP";
      }

      // If we get here then we can search for duplicates.
      // Look AFTER the IP header for dupliate IP.

      // Search to see if a MAC is repeated.
      bool may_have_dup = ip_types.isDupMAC(static_cast<int>(next_header), tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupMAC(static_cast<int>(next_header), tcp_source_port));
      //std::cout << "May MAC Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
      if (!may_have_dup) {
        uint32_t search_len = pkt_header->len - final_data_offset;
        const uint8_t* data_loc = (pkt_data + final_data_offset);
        if (generateCSV) {
          std::map<int, std::set<int> > matches = search_macs.findAllMatches(data_loc, search_len);
          num_mac_found_unique += matches.size();
          num_mac_found_total += search_macs.countMatches(matches);
        } else if (search_macs.findFirstMatch(data_loc, search_len) >= 0) {
          pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
          it++;
          ++output_row;
          continue;
        }
      }

      // If NOT generating the CSV, do not search for IP if
      // a MAC has already been found.
      if (generateCSV || (num_mac_found_unique == 0)) {
        may_have_dup = ip_types.isDupIP(static_cast<int>(next_header), tcp_destination_port) || (tcp_destination_port != tcp_source_port && ip_types.isDupIP(static_cast<int>(next_header), tcp_source_port));
        //std::cout << "May  IP Dup: " << may_have_dup << " proto:" << (int) ipHeader->ip_p << " source:" << tcp_source_port << " dest:" << tcp_destination_port << std::endl;
        if (!may_have_dup) {
          uint32_t search_len = pkt_header->len - final_data_offset;
          const uint8_t* data_loc = (pkt_data + final_data_offset);
          if (generateCSV) {
            std::map<int, std::set<int> > matches_4 = search_ipv4.findAllMatches(data_loc, search_len);
            std::map<int, std::set<int> > matches_6 = search_ipv6.findAllMatches(data_loc, search_len);
            num_ip_found_unique += matches_4.size();
            num_ip_found_unique += matches_6.size();
            num_ip_found_total += search_ipv4.countMatches(matches_4);
            num_ip_found_total += search_ipv6.countMatches(matches_6);
          } else if (
            (search_ipv4.findFirstMatch(data_loc, search_len) >= 0) ||
            (search_ipv6.findFirstMatch(data_loc, search_len) >= 0)) {
            pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
            it++;
            ++output_row;
            continue;
          }
        }
      }

      if (num_mac_found_unique > 0 || num_ip_found_unique > 0) {
        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        if (test_mode)
          std::cout << it << " DUP MAC: " << num_mac_found_unique << " IPv6:" << num_ip_found_unique << " with Ethertype IPv4 protocol = " << proto << " (" << ip_p << ")" << " ports: " << tcp_source_port << " / " << tcp_destination_port << std::endl;

        if (generateCSV) {
          if (!has_header) {
            *csv << heading;
            csv->endRow();
            has_header = true;
          }
          *csv << output_row
              << ip_str_source
              << ip_str_dest
              << num_ip_found_unique
              << num_mac_found_unique
              << num_ip_found_total
              << num_mac_found_total;
          csv->endRow();
        }
        num_ip_found_unique = 0;
        num_mac_found_unique = 0;
        num_ip_found_total = 0;
        num_mac_found_total = 0;

        it++;
        ++output_row;
        continue;
      }

      // We are done with this ipv6 packet. No need to search more.
      // The packet does not contain dupliate data!
      if (test_mode && verbose)
        std::cout << it << " Skipping packet with Ethertype IPv6 protocol = " << proto << " (" << static_cast<int>(next_header) << ")" << " ports: " << tcp_source_port << " / " << tcp_destination_port << std::endl;
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

      if (generateCSV) {
        std::map<int, std::set<int> > matches = search_macs.findAllMatches(data_loc, search_len);
        num_mac_found_unique += matches.size();
        num_mac_found_total += search_macs.countMatches(matches);
      } else if (search_macs.findFirstMatch(data_loc, search_len) >= 0) {
        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        ++output_row;
        continue;
      }
    }

    if (!ethernet_types.isDupIP(ether_type_int)) {
      if (generateCSV) {
        std::map<int, std::set<int> > matches_4 = search_ipv4.findAllMatches(data_loc, search_len);
        std::map<int, std::set<int> > matches_6 = search_ipv6.findAllMatches(data_loc, search_len);
        num_ip_found_unique += matches_4.size();
        num_ip_found_unique += matches_6.size();
        num_ip_found_total += search_ipv4.countMatches(matches_4);
        num_ip_found_total += search_ipv6.countMatches(matches_6);
      } else if (
        (search_ipv4.findFirstMatch(data_loc, search_len) >= 0) || 
        (search_ipv4.findFirstMatch(data_loc, search_len) >= 0)) {
        pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
        it++;
        ++output_row;
        continue;
      }
    }

    if (num_mac_found_unique > 0 || num_ip_found_unique > 0) {
      pcap_dump( (u_char *)dumpfile, pkt_header, pkt_data);
      if (m_verbose)
        std::cout << it << " DUP MAC: " << num_mac_found_unique << " IP:" << num_ip_found_unique << " with Ethertype = " << ether_type_int << std::endl;

      if (!has_header) {
        *csv << heading;
        csv->endRow();
        has_header = true;
      }
      *csv << output_row
          << ip_str_source
          << ip_str_dest
          << num_ip_found_unique
          << num_mac_found_unique
          << num_ip_found_total
          << num_mac_found_total;
      csv->endRow();
      num_ip_found_unique = 0;
      num_mac_found_unique = 0;
      num_ip_found_total = 0;
      num_mac_found_total = 0;

      it++;
      ++output_row;
      continue;
    }

    // Done with all processing for this packet.
    it++;
  }


  pcap_close(pcap_file);
  pcap_dump_close(dumpfile);
  std::cout << "Thread " << m_thread_id << " Examined "<< it <<" packets in file " << pcap_filename << std::endl;

  return 0;
}