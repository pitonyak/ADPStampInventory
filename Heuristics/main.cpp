
#include <getopt.h>
#include <iomanip>
#include <iostream>

#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "process_pcap.h"
#include "utilities.h"

// The MAC and IP addresses in this file.
MacAddresses mac_addresses;
IpAddresses ip_addresses;
MacAddresses dest_mac_to_ignore;
std::string extra_name = "anomaly";
std::string output_directory = "";
static int test_flag;
static int verbose_flag;

void usage(){
  std::cout << "Usage" << std::endl;
  std::cout << "  --help    Print usage instructions." << std::endl;
  std::cout << "  --verbose Turn on verbose mode." << std::endl;
  std::cout << "  --brief   Turn off verbose mode." << std::endl;
  std::cout << "  --test    Turn on test mode." << std::endl;
  std::cout << "  --heuristic Generate the heuristic anomaly file (no CSV)." << std::endl;
  std::cout << "  --csv     Generate the CSV and the anomaly file." << std::endl;
  std::cout << "  --pcap <NAME> Full path to the PCAP file to read." << std::endl;
  std::cout << "  --directory <NAME> Output directory where files are stored." << std::endl;
  std::cout << "            defaults to the directory containing the PCAP ile." << std::endl;
  std::cout << "  --name <NAME> By default, file.pcap generates file.anomaly.pcap" << std::endl;
  std::cout << "            use --name bob to generate file.bob.pcap instead.  " << std::endl;
  std::cout << "  --ipmac Force creation of the IP and MAC files." << std::endl;
  std::cout << "  --macsec Strip MACSEC and VLAN and place in a new PCAP." << std::endl;
  std::cout << "  --min_ip_matches Min num of unique IPs to write a packet to anomaly file and CSV. Defaults to 2." << std::endl;
  std::cout << "  --min_mac_matches Min num of unique IPs to write a packet to anomaly file and CSV. Defaults to 2." << std::endl;
  std::cout << std::endl;
  std::cout << "  -? Shortened version of --help" << std::endl;
  std::cout << "  -h Shortened version of --heuristic" << std::endl;
  std::cout << "  -i Shortened version of --ipmac" << std::endl;
  std::cout << "  -c Shortened version of --csv" << std::endl;
  std::cout << "  -p Shortened version of --pcap" << std::endl;
  std::cout << "  -d Shortened version of --directory" << std::endl;
  std::cout << "  -m Shortened version of --macsec" << std::endl;
  std::cout << "  -n Shortened version of --name" << std::endl;
  std::cout << std::endl;
  std::cout << "All filenames are generated, you cannot choose them." << std::endl;
  std::cout << "CSV file and Anomaly file are over-written." << std::endl;
  std::cout << "IP and MAC files are used if they exist and created if they do not." << std::endl;
  std::cout << "Generating a CSV may take about 300 times longer than just the anomaly file." << std::endl;
  std::cout << std::endl;
}

int main(int argc, char **argv){

  int min_ip_matches = 2;
  int min_mac_matches = 2;
  std::string temp_string;

  IPTypes ip_types;
  ip_types.readProtocols("ip_protocols.txt", false, 10);
  ip_types.readProtocolPorts("ip_protocol_ports.txt");
  //std::cout << ip_types;
  //std::cout << std::endl;

  EthernetTypes ethernet_types;
  ethernet_types.read("eth_types.txt");
  dest_mac_to_ignore.read_file("destination_macs.txt");
  //std::cout << ethernet_types;
  //std::cout << std::endl;

  test_flag = 0;
  verbose_flag = 0;
  bool create_anomaly_list = false;
  bool create_anomaly_csv  = false;
  bool create_mac_ip_file  = false;
  bool remove_mac_sec = false;
  std::string pcap_filename = "";;
  int c;
  while (1) {
    static struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose",   no_argument,       &verbose_flag, 1},
      {"brief",     no_argument,       &verbose_flag, 0},
      {"test",      no_argument,       &test_flag, 1},
      /* Can I set an int directly */
      {"notest",    no_argument,       &test_flag, 0},
      /* These options don’t set a flag. We distinguish them by their indices. */
      {"help",      no_argument,       0, '?'},
      {"heuristic", no_argument,       0, 'h'},
      {"csv",       no_argument,       0, 'c'},
      {"ipmac",     no_argument,       0, 'i'},
      {"pcap",      required_argument, 0, 'p'},
      {"directory", required_argument, 0, 'd'},
      {"name",      required_argument, 0, 'n'},
      {"macsec",    no_argument,       0, 'm'},
      {"min_ip_matches",  required_argument, 0, 'x'},
      {"min_mac_matches", required_argument, 0, 'y'},
      {0, 0, 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "?ihcp:d:n:m", long_options, &option_index);

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
        create_anomaly_list = true;
        break;

      case 'i':
        create_mac_ip_file = true;
        break;

      case 'c':
        create_anomaly_csv = true;
        break;

      case 'p':
        pcap_filename = optarg ? optarg : "";
        break;

      case 'd':
        output_directory = optarg ? optarg : "";
        break;

      case 'm':
        remove_mac_sec = true;
        break;

      case 'n':
        extra_name = optarg ? optarg : "";
        break;

      case 'x':
        temp_string = optarg ? optarg : "";
        if (!temp_string.empty()) {
          min_ip_matches = std::stoi(temp_string);
        }
        break;

      case 'y':
        temp_string = optarg ? optarg : "";
        if (!temp_string.empty()) {
          min_mac_matches = std::stoi(temp_string);
        }
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

  std::atomic_bool* abort_requested = nullptr;
  // Lets look at the default IP and MAC file names.
  // the pcap_fname probably ends with ".pcap" so lets
  // create the file name "<base_name>.ip.txt" and "<base_name>.mac.txt"
  if (create_mac_ip_file) {
    // This will force a new file to be written.
    std::string out_mac_fname = getHeuristicFileName(pcap_filename, MAC_Type, output_directory, extra_name);
    std::string out_ip_fname = getHeuristicFileName(pcap_filename, IP_Type, output_directory, extra_name);
    write_ip_and_mac_from_pcap(mac_addresses, ip_addresses, pcap_filename, out_mac_fname, out_ip_fname, abort_requested);
  }

  if (create_anomaly_csv) {
    std::cout << "Creating Anomaly and CSV File" << std::endl;
    create_heuristic_anomaly_csv(dest_mac_to_ignore, mac_addresses, ip_addresses, ethernet_types, ip_types, pcap_filename, output_directory, extra_name, verbose_flag, create_anomaly_csv, abort_requested, min_ip_matches, min_mac_matches);
  } else if (create_anomaly_list) {
    std::cout << "Creating Anomaly File" << std::endl;
    create_heuristic_anomaly_csv(dest_mac_to_ignore, mac_addresses, ip_addresses, ethernet_types, ip_types, pcap_filename, output_directory, extra_name, verbose_flag, create_anomaly_csv, abort_requested, min_ip_matches, min_mac_matches);
  }

  if (remove_mac_sec) {
    strip_macsec_vlan_frames(ethernet_types, pcap_filename, output_directory, extra_name, verbose_flag, abort_requested);
  }

  return 0;
}
