#ifndef HEURISTIC_THREAD_POOL_H
#define HEURISTIC_THREAD_POOL_H

#include <atomic>

#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"


//**************************************************************************
//! Read the pcap file and create a new IP and MAC file.
/*!
 * This ALWAYS writes new files overwriting any existing file.
 * Use read_create_mac_ip_files if you want to read the file if it 
 * already exists and create it if it does not.
 * 
 * @see read_create_mac_ip_files()
 * 
 * \param [in] pcap_filename Full path to the input PCAP file
 * 
 * \param [in] out_mac_fname Filename for the MACs.
 * 
 * \param [in] out_ip_fname Filename for the IPs
 * 
 * \returns 0 on no error, not very useful at this time.
 *
 ***************************************************************************/
int write_ip_and_mac_from_pcap(MacAddresses& mac_addresses, IpAddresses& ip_addresses, const std::string& pcap_filename, const std::string& out_mac_fname, const std::string& out_ip_fname, std::atomic_bool* abort_requested);

//**************************************************************************
//! Create the IP and MAC text files if needed. If they already exist, read them.
/*!
 * @see write_ip_and_mac_from_pcap()
 * @see getAnomalyFileName()
 * 
 * Given the full path to a PCAP file, generate the file name for the 
 * list of IP and MAC addresses. 
 * 
 * Generate Warnings if the user does not have appropriate access to
 * read or write. The problem is igored, which may cause a core dump,
 * but the warning indicates why things failed.
 * 
 * On exit, ip_addresses and mac_addresses are populated with
 * the IP and MAC addresses in the file.
 * 
 * \param [in] pcap_filename - Filename of the PCAP file.
 *
 ***************************************************************************///
void read_create_mac_ip_files(MacAddresses& mac_addresses, IpAddresses& ip_addresses, const std::string& pcap_filename, std::string output_directory, std::string extra_heuristic_name, std::atomic_bool* abort_requested);

//**************************************************************************
//! Create the Anomaly and the CSV file. IP and MAC files are created as needed.
/*!
 * 
 * Warnings are printed if it looks like a file cannot be read or if a directory
 * is not readable, but the problem is ignored. This may cause a core dump, 
 * but the initial warning will let you know why things failed.
 * 
 * On exit, ip_addresses and mac_addresses will be populated with
 * all of the IP and MAC addresses in the file.
 * 
 * \param [in] ethernet_types
 *
 * \param [in] ip_types
 *
 * \param [in] pcap_filename - Filename of the PCAP file.
 *
 * \param [in] verbose
 *
 * \param [in] generateCSV If True, generate a CSV file, otherwise do not create a CSV file.
 *
 * \returns 0 on no error, -1 otherwise.
 * 
 ***************************************************************************///
int create_heuristic_anomaly_csv(MacAddresses& dest_mac_to_ignore, MacAddresses& mac_addresses, IpAddresses& ip_addresses, const EthernetTypes& ethernet_types, const IPTypes& ip_types, const std::string& pcap_filename, std::string output_directory, std::string extra_heuristic_name, bool verbose, bool generateCSV, std::atomic_bool* abort_requested);


#endif