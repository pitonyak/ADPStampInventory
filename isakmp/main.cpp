#include <cerrno>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <getopt.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap.h>

#include "ip_address_pairs.h"
#include "isakmp.h"
#include "string_util.h"

using pcap_handle = std::unique_ptr<pcap_t, decltype(&pcap_close)>;
using pcap_dump_handle = std::unique_ptr<pcap_dumper_t, decltype(&pcap_dump_close)>;

/**
 * @brief Flags for selecting when to show information for skipped packets due to unsupported/unrecognized values.
 */
struct show_skip_flags
{
    /// Show information for packets skipped due to having an ethernet type that is neither IPv4 nor IPv6.
    static constexpr uint32_t EtherType = (1 << 0);
    /// Show information for packets skipped due to fragmentation.
    static constexpr uint32_t Fragment = (1 << 1);
    /// Show information for packets skipped due to them being IPv6.
    static constexpr uint32_t IPv6 = (1 << 2);
    /// Show information for packets skipped due to ISAKMP encrypted payload flag.
    static constexpr uint32_t ISAKMPEncypt = (1 << 3);
    /// Show information for packets skipped due to ISAKMP length value that cannot fit in packet.
    static constexpr uint32_t ISAKMPLength = (1 << 4);
    /// Show information for packets skipped due to non-ISAKMP ports.
    static constexpr uint32_t Port = (1 << 5);
    /// Show information for packets skipped due to non-UDP IP protocol.
    static constexpr uint32_t Protocol = (1 << 6);
    /// Show information for all skipped packets.
    static constexpr uint32_t All = UINT32_MAX;
};

/**
 * @brief Reads a 6-byte sequence from the given file path which will be used to match against the end of the vendor IDs
 * in ISAKMP packets.
 *
 * The byte sequence is expected to be of the form "xx:xx:xx:xx:xx:xx", where each 'x' represents a hexadecimal digit.
 *
 * @param[in] vendor_file_path The path to the file from which to read a hexadecimal byte sequence.
 * @return A vector of byte values if the 6-byte sequence could be successfully read from the file; an empty vector
 * otherwise.
 */
std::vector<uint8_t> get_vendor_bytes(const std::string &vendor_file_path)
{
    std::ifstream vendor_file(vendor_file_path);

    if (!vendor_file)
    {
        std::cerr << "Failed to open vendor file at " << vendor_file_path << ": " << strerror(errno) << std::endl;
        return {};
    }

    size_t line_count = 0;
    std::vector<uint8_t> vendor_bytes;
    std::string line;
    while (std::getline(vendor_file, line))
    {
        line_count++;

        // Remove trailing and leading whitespace from each line read from the file.
        trim(line);

        // Skip blank lines and comments.
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Skip lines that aren't of the expected character length for a 6-byte sequence.
        constexpr size_t expected_byte_sequence_length = 17;
        if (line.size() != expected_byte_sequence_length)
        {
            continue;
        }

        // Parse the byte sequence with the expected colon delimiter.
        std::istringstream byte_string(line);
        std::string token;
        constexpr char byte_separator = ':';
        while (std::getline(byte_string, token, byte_separator))
        {
            constexpr size_t expected_byte_chars = 2;
            if (token.size() != expected_byte_chars)
            {
                std::cerr << "Unexpected length of " << token.size() << " for byte \"" << token << "\" on line "
                          << line_count << std::endl;
                return {};
            }

            try
            {
                size_t chars_processed;
                const uint8_t byte = std::stoul(token, &chars_processed, 16);
                if (chars_processed != expected_byte_chars)
                {
                    std::cerr << "Unexpected number of hex characters for byte on line " << line_count << std::endl;
                    return {};
                }
                vendor_bytes.push_back(byte);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid hex characters in byte sequence on line " << line_count << std::endl;
                return {};
            }
            catch (const std::exception &e)
            {
                std::cerr << "Unexpected error while parsing byte sequence on line " << line_count << ": " << e.what()
                          << std::endl;
                return {};
            }
        }
    }

    return vendor_bytes;
}

/**
 * @brief Outputs the given number of bytes in hexadecial and separated by spaces.
 *
 * @param[in] data Pointer to data to dump.
 * @param[in] len The number of bytes to dump from the given address.
 */
void dump_bytes(const uint8_t *data, size_t len)
{
    size_t bytes_dumped = 0;
    while (bytes_dumped < len)
    {
        const int byte_value = data[bytes_dumped++];
        std::cout << std::hex << std::setfill('0') << std::setw(2) << byte_value << " ";
    }
    std::cout << std::dec << std::endl;
}

/**
 * @brief Searches the given pcap file for ISAKMP packets that contain vendor ID payloads that end with the specified
 * matching set of bytes and records the source and destination IP addresses associated with these packets.
 *
 * @param[in,out] pcap_file The pcap file handle for which to search for ISAKMP packets.
 * @param[in] vendor_match Vector of bytes to match in the vendor id payloads of ISAKMP packets.
 * @param[in,out] address_set The object in which to record source and destination IP addresses for matching ISAKMP
 * packets.
 * @param[in] verbose_output_enabled Flag indicating whether verbose output is enabled for the program.
 * @param[in] hex_dump_enabled Flag indicating whether suspected ISAKMP packets should be dumped as a set of hexadecimal
 * bytes.
 * @param[in] skips_flags Flags indicating whether information about skipped packets should be displayed.
 */
void find_address_pairs(pcap_handle &pcap_file, const std::vector<uint8_t> &vendor_match, IPAddressPairSet &address_set,
                        bool verbose_output_enabled, bool hex_dump_enabled, uint32_t skip_flags)
{
    struct pcap_pkthdr *packet_header;
    const uint8_t *packet_data;
    size_t total_packet_count = 0;

    while (true)
    {
        int pcap_result = pcap_next_ex(pcap_file.get(), &packet_header, &packet_data);
        if (pcap_result == PCAP_ERROR_BREAK)
        {
            if (verbose_output_enabled)
            {
                std::cout << "Done reading " << total_packet_count << " packets from input pcap file.\n";
            }
            break;
        }
        else if (pcap_result == PCAP_ERROR)
        {
            pcap_perror(pcap_file.get(), "Error encountered while reading packets from pcap");
            continue;
        }
        total_packet_count++;

        const auto *ethernet_header = reinterpret_cast<const struct ether_header *>(packet_data);
        const uint16_t ethernet_type = ntohs(ethernet_header->ether_type);
        if (ethernet_type == ETHERTYPE_IP)
        {
            const auto *ip_header = reinterpret_cast<const struct ip *>(packet_data + sizeof(struct ether_header));

            // Mask bits for fragment offset and More Fragments flag. A packet should be considered a fragment if the
            // More Fragments flag is set or if the fragment offset is non-zero.
            const uint16_t ip_offset_field = ntohs(ip_header->ip_off);
            if ((ip_offset_field & (IP_MF | IP_OFFMASK)) > 0)
            {
                // TODO: Handle fragmented packets. Ignore such packets for now.
                if (verbose_output_enabled && (skip_flags & show_skip_flags::Fragment))
                {
                    bool reserved = ip_offset_field & IP_RF;
                    bool dont_fragment = ip_offset_field & IP_DF;
                    bool more_fragments = ip_offset_field & IP_MF;
                    uint16_t fragment_offset = ip_offset_field & IP_OFFMASK;
                    std::cout << std::boolalpha << "Packet #" << total_packet_count << " skipped due to fragmentation\n"
                              << "\tReserved: " << reserved << "\n\tDon't Fragment: " << dont_fragment
                              << "\n\tMore Fragments: " << more_fragments << "\n\tFragment Offset: " << fragment_offset
                              << std::endl;
                }
                continue;
            }

            if (ip_header->ip_p != IPPROTO_UDP)
            {
                // TODO: Investigate whether ISAKMP traffic can be sent over TCP and handle that.
                if (verbose_output_enabled && (skip_flags & show_skip_flags::Protocol))
                {
                    std::cout << "Packet #" << total_packet_count
                              << " skipped due to non-UDP IP protocol: " << static_cast<int>(ip_header->ip_p)
                              << std::endl;
                }
                continue;
            }

            // Get offset to IP payload and UDP header.
            const size_t offset_to_ip_payload = sizeof(struct ether_header) + ip_header->ip_hl * sizeof(uint32_t);
            const auto *udp_header = reinterpret_cast<const struct udphdr *>(packet_data + offset_to_ip_payload);

            // Get UDP source/destination ports.
            uint16_t source_port = ntohs(udp_header->uh_sport);
            uint16_t destination_port = ntohs(udp_header->uh_dport);

            // ISAKMP traffic is expected on ports 500 and 4500.
            constexpr uint16_t isakmp_port = 500;
            constexpr uint16_t isakmp_nat_port = 4500;
            bool should_be_encapsulated_esp = false;
            if (source_port != isakmp_port && destination_port != isakmp_port)
            {
                if (source_port == isakmp_nat_port || destination_port == isakmp_nat_port)
                {
                    // Indicate that the packet should be encapsulated ESP based on the fact that the port is 4500.
                    should_be_encapsulated_esp = true;
                }
                else
                {
                    // Skip packets from other ports.
                    if (verbose_output_enabled && (skip_flags & show_skip_flags::Port))
                    {
                        std::cout << "Packet #" << total_packet_count << " skipped due to non-ISAKMP ports\n"
                                  << "\tSource Port: " << source_port << "\n\tDestination Port: " << destination_port
                                  << std::endl;
                    }
                    continue;
                }
            }

            // Get offset to UDP payload and the ISAKMP header.
            const size_t offset_to_udp_payload = offset_to_ip_payload + sizeof(struct udphdr);
            const struct isakmp_header *isakmp_header;
            bool is_encapsulated_esp = false;

            // Check for Non-ESP marker. See https://www.rfc-editor.org/rfc/rfc3948#section-2.2.
            if (should_be_encapsulated_esp &&
                *reinterpret_cast<const uint32_t *>(packet_data + offset_to_udp_payload) == 0)
            {
                // ISAKMP header is after the non-ESP marker.
                is_encapsulated_esp = true;
                isakmp_header = reinterpret_cast<const struct isakmp_header *>(packet_data + offset_to_udp_payload +
                                                                               sizeof(uint32_t));
            }
            else
            {
                // ISAMKP header is directly after UDP header.
                isakmp_header = reinterpret_cast<const struct isakmp_header *>(packet_data + offset_to_udp_payload);
            }

            // Skip packets where the ISAKMP length cannot possibly fit in what remains of the packet.
            const size_t isakmp_length = ntohl(isakmp_header->length);
            const size_t remaining_packet_length = packet_header->len - (reinterpret_cast<uintptr_t>(isakmp_header) -
                                                                         reinterpret_cast<uintptr_t>(packet_data));
            if (isakmp_length > remaining_packet_length)
            {
                if (verbose_output_enabled && (skip_flags & show_skip_flags::ISAKMPLength))
                {
                    std::cout << "Packet " << total_packet_count << " is likely not ISAKMP\n"
                              << "ISAKMP length " << isakmp_length << " > remaining packet length of "
                              << remaining_packet_length << std::endl;
                }
                continue;
            }

            if (verbose_output_enabled && hex_dump_enabled)
            {
                std::cout << "Packet #" << total_packet_count << " is ISAKMP:\n";
                dump_bytes((uint8_t *)isakmp_header, isakmp_length);
            }

            // Skip packets with encrypted payloads for IKEv1.
            if (isakmp_header->flags & isakmp_flags::IKE_Encrypt)
            {
                if (verbose_output_enabled && (skip_flags & show_skip_flags::ISAKMPEncypt))
                {
                    std::cout << "Packet #" << total_packet_count << " skipped due to encrypted payload" << std::endl;
                }
                continue;
            }

            // Get offset to ISAKMP payload(s).
            const size_t offset_to_isakmp_payload =
                offset_to_udp_payload + sizeof(struct isakmp_header) + (is_encapsulated_esp ? sizeof(uint32_t) : 0);
            const auto *current_payload =
                reinterpret_cast<const struct isakmp_generic_payload_header *>(packet_data + offset_to_isakmp_payload);
            isakmp_payload_type current_payload_type = isakmp_header->next_payload;

            while (current_payload_type != isakmp_payload_type::None &&
                   current_payload_type != isakmp_payload_type::IKE2_Encrypted)
            {
                // We need a mininmum of 6 bytes, so the vendor will be ignored if the payload length is less than that.
                constexpr uint16_t vendor_payload_length_min = 6;
                const uint16_t current_payload_length = ntohs(current_payload->length);

                // Look for Vendor ID payloads. The payload type has a different value depending on whether this is
                // IKEv1 or IKEv2. The vendor payload length must also be >= 6.
                if ((current_payload_type == isakmp_payload_type::IKE_VendorId ||
                     current_payload_type == isakmp_payload_type::IKE2_VendorId) &&
                    current_payload_length > sizeof(isakmp_generic_payload_header) &&
                    current_payload_length - sizeof(isakmp_generic_payload_header) >= vendor_payload_length_min)
                {
                    if (verbose_output_enabled && hex_dump_enabled)
                    {
                        std::cout << "ISAKMP Vendor: ";
                        dump_bytes(reinterpret_cast<const uint8_t *>(current_payload) +
                                       sizeof(isakmp_generic_payload_header),
                                   current_payload_length - sizeof(isakmp_generic_payload_header));
                    }

                    // Determine if the end of vendor id matches the desired byte sequence.
                    const auto *vendor_id_suffix = reinterpret_cast<const uint8_t *>(current_payload) +
                                                   current_payload_length - vendor_payload_length_min;
                    if (memcmp(vendor_id_suffix, vendor_match.data(), vendor_match.size()) == 0)
                    {
                        address_set.add_pair(ip_header->ip_src, ip_header->ip_dst);
                        break;
                    }
                }

                // Get next payload and its type.
                current_payload_type = current_payload->next_payload;
                current_payload = reinterpret_cast<const struct isakmp_generic_payload_header *>(
                    reinterpret_cast<const uint8_t *>(current_payload) + current_payload_length);
            }
        }
        else if (ethernet_type == ETHERTYPE_IPV6)
        {
            // TODO: Handle IPv6.
            if (verbose_output_enabled && (skip_flags & show_skip_flags::IPv6))
            {
                std::cout << "Packet #" << total_packet_count << " ignored because it is IPv6\n";
            }
        }
        else if (verbose_output_enabled && (skip_flags & show_skip_flags::EtherType))
        {
            std::cout << "Packet #" << total_packet_count << " ignored due to unsupported ethertype 0x" << std::hex
                      << std::setfill('0') << std::setw(4) << ethernet_type << std::dec << "\n";
        }
    }
}

/**
 * @brief Outputs packets from the given pcap file that were recorded in the given set of source and destination IP
 * addresses into a separate output file.
 *
 * @param[in,out] pcap_file The pcap file handle from which to dump packets.
 * @param[in,out] pcap_output_file The handle of the output pcap file.
 * @param[in] address_set The set of source and destination IP addresses from which matching ISAKMP packets were sent.
 * @param[in] verbose_output_enabled Flag indicating whether verbose output is enabled for the program.
 */
void dump_matching_packets(pcap_handle &pcap_file, pcap_dump_handle &pcap_output_file,
                           const IPAddressPairSet &address_set, bool verbose_output_enabled)
{
    struct pcap_pkthdr *packet_header;
    const uint8_t *packet_data;
    size_t total_packet_count = 0;
    size_t written_packet_count = 0;
    while (true)
    {
        int pcap_result = pcap_next_ex(pcap_file.get(), &packet_header, &packet_data);
        if (pcap_result == PCAP_ERROR)
        {
            pcap_perror(pcap_file.get(), "Error encountered while reading packets from pcap");
            continue;
        }
        else if (pcap_result == PCAP_ERROR_BREAK)
        {
            if (verbose_output_enabled)
            {
                std::cout << "Wrote " << written_packet_count << " / " << total_packet_count
                          << " packets to output file.\n";
            }
            break;
        }
        total_packet_count++;

        const auto *ethernet_header = reinterpret_cast<const ether_header *>(packet_data);
        const uint16_t ethernet_type = ntohs(ethernet_header->ether_type);
        if (ethernet_type == ETHERTYPE_IP)
        {
            const auto *ip_header = reinterpret_cast<const struct ip *>(packet_data + sizeof(struct ether_header));

            // Check if IPv4 source/destination addresses were recorded in the address set.
            if (address_set.has_pair(ip_header->ip_src, ip_header->ip_dst))
            {
                // Write packet to output file.
                pcap_dump(reinterpret_cast<u_char *>(pcap_output_file.get()), packet_header, packet_data);
                written_packet_count++;
            }
        }
        // TODO: Handle IPv6 using constant ETHERTYPE_IPV6.
    }
}

/**
 * @brief Displays information on the options that can be passed on the command line for this program.
 *
 * @param out The output stream in which to write the usage information.
 * @param program_name The string used to invoke the program.
 */
void print_usage(std::ostream &out, const std::string &program_name)
{
    out << "Usage: " << program_name << " [options] -r <infile> -w <outfile>\n\n";
    out << "  -r, --read-file=FILE   Set input pcap filename\n";
    out << "  -w, --write-file=FILE  Set output pcap filename\n";
    out << "  -m, --match-file=FILE  Set filename to read vendor id byte sequence to match against ISAKMP packets.\n";
    out << "                         If not specified, vendor_match.txt will be used from the current directory\n";
    out << "  -v, --verbose          Display verbose output\n";
    out << "  -d, --dump-hex         Dump hex data for ISAKMP packets in verbose mode\n";
    out << "  -s, --show-skips=TYPE  Display information in verbose mode about packets which were skipped due to\n";
    out << "                         the following unsupported/unrecognized attribute types:\n";
    out << "                           ethertype, fragment, ipv6, isakmp-encrypt, isakmp-length, port, protocol,\n";
    out << "                           all\n";
    out << "  -h, --help             Display this help and exit" << std::endl;
}

int main(int argc, char **argv)
{
    // Define set of options that can be passed on the command line for this program.
    static struct option long_options[] = {
        {"dump-hex",   no_argument,       0,       'd'},
        {"help",       no_argument,       0,       'h'},
        {"match-file", required_argument, 0,       'm'},
        {"read-file",  required_argument, 0,       'r'},
        {"show-skips", required_argument, 0,       's'},
        {"verbose",    no_argument,       0,       'v'},
        {"write-file", required_argument, 0,       'w'},
        {nullptr,      0,                 nullptr, 0  },
    };

    std::string input_file, output_file, match_file;
    bool verbose_output_enabled = false;
    bool hex_dump_enabled = false;
    uint32_t skip_flags = 0;
    int option, option_index;
    while ((option = getopt_long(argc, argv, "dhr:m:s:vw:", long_options, &option_index)) != -1)
    {
        switch (option)
        {
        case 'd':
            hex_dump_enabled = true;
            break;
        case 'h':
            std::cout << "Filter packets from devices sending ISAKMP packets containing a particular vendor id.\n\n";
            print_usage(std::cout, argv[0]);
            exit(EXIT_SUCCESS);
            break;
        case 'm':
            match_file = optarg;
            break;
        case 'r':
            input_file = optarg;
            break;
        case 's':
            if (strcasecmp(optarg, "ethertype") == 0)
            {
                skip_flags |= show_skip_flags::EtherType;
            }
            else if (strcasecmp(optarg, "fragment") == 0)
            {
                skip_flags |= show_skip_flags::Fragment;
            }
            else if (strcasecmp(optarg, "ipv6") == 0)
            {
                skip_flags |= show_skip_flags::IPv6;
            }
            else if (strcasecmp(optarg, "isakmp-encrypt") == 0)
            {
                skip_flags |= show_skip_flags::ISAKMPEncypt;
            }
            else if (strcasecmp(optarg, "isakmp-length") == 0)
            {
                skip_flags |= show_skip_flags::ISAKMPLength;
            }
            else if (strcasecmp(optarg, "port") == 0)
            {
                skip_flags |= show_skip_flags::Port;
            }
            else if (strcasecmp(optarg, "protocol") == 0)
            {
                skip_flags |= show_skip_flags::Protocol;
            }
            else if (strcasecmp(optarg, "all") == 0)
            {
                skip_flags = show_skip_flags::All;
            }
            else
            {
                std::cerr << "Invalid argument given to --show-skips option: " << optarg << "\n\n";
                print_usage(std::cerr, argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'v':
            verbose_output_enabled = true;
            break;
        case 'w':
            output_file = optarg;
            break;
        case '?':
            if (optopt == 's')
            {
                std::cerr << "See possible arguments to --show-skips option below.\n\n";
                print_usage(std::cerr, argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            std::cerr << "Unrecognized option: " << option << "\n\n";
            print_usage(std::cerr, argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (input_file.empty())
    {
        std::cerr << "Input pcap file required. Please specify the --read-file option.\n\n";
        print_usage(std::cerr, argv[0]);
        return EXIT_FAILURE;
    }

    if (output_file.empty())
    {
        std::cerr << "Output pcap file is required. Please specify the --write-file option.\n\n";
        print_usage(std::cerr, argv[0]);
        return EXIT_FAILURE;
    }

    if (match_file.empty())
    {
        match_file = "vendor_match.txt";
    }

    // Read vendor bytes to match from file.
    const std::vector<uint8_t> vendor_match = get_vendor_bytes(match_file);
    if (vendor_match.size() == 0)
    {
        std::cerr << "Failed to read vendor id byte sequence from " << match_file << std::endl;
        return EXIT_FAILURE;
    }

    if (verbose_output_enabled)
    {
        std::cout << "ISAKMP vendor ID bytes to match: " << std::hex;
        for (size_t vendor_byte_index = 0; vendor_byte_index < vendor_match.size() - 1; vendor_byte_index++)
        {
            std::cout << std::setfill('0') << std::setw(2) << static_cast<short>(vendor_match[vendor_byte_index])
                      << ':';
        }
        std::cout << std::setw(2) << static_cast<short>(vendor_match[vendor_match.size() - 1]) << std::dec << std::endl;
    }

    // Open input pcap file.
    char pcap_open_err[PCAP_ERRBUF_SIZE];
    pcap_handle pcap_file(pcap_open_offline(input_file.c_str(), pcap_open_err), &pcap_close);
    if (!pcap_file)
    {
        std::cerr << "Failed to open input pcap file: " << pcap_open_err << std::endl;
        return EXIT_FAILURE;
    }

    if (verbose_output_enabled)
    {
        std::cout << "Opened input pcap file " << input_file << std::endl;
    }

    // Ensure that the pcap file only has Ethernet packets
    if (pcap_datalink(pcap_file.get()) != DLT_EN10MB)
    {
        std::cerr << "PCAP file " << input_file << " is not an Ethernet capture." << std::endl;
        return EXIT_FAILURE;
    }

    // Find source/destination address pairs that are sending ISAKMP packets with matching vendor id.
    IPAddressPairSet address_set;
    find_address_pairs(pcap_file, vendor_match, address_set, verbose_output_enabled, hex_dump_enabled, skip_flags);

    std::cout << "Found " << address_set.size() << " unique IP address pairs from matching ISAKMP packets.\n";

    // Bail out early if no matching ISAKMP packets were found in the input pcap file.
    if (address_set.size() == 0)
    {
        std::cout << "Skipped writing filtered packets to output file because no ISAKMP packets matched.\n";
        return EXIT_SUCCESS;
    }

    // Re-open pcap file to look at packets from the beginning again.
    pcap_file.reset(pcap_open_offline(input_file.c_str(), pcap_open_err));
    if (!pcap_file)
    {
        std::cerr << "Failed to open input pcap file: " << pcap_open_err << std::endl;
        return EXIT_FAILURE;
    }

    // Open output pcap file for writing.
    pcap_dump_handle pcap_output_file(pcap_dump_open(pcap_file.get(), output_file.c_str()), &pcap_dump_close);
    if (!pcap_output_file)
    {
        pcap_perror(pcap_file.get(), "Failed to open output pcap file");
        return EXIT_FAILURE;
    }

    // Write out packets from source/destination IP addresses which sent ISAKMP packets with matching vendor id.
    dump_matching_packets(pcap_file, pcap_output_file, address_set, verbose_output_enabled);
    std::cout << "Wrote filtered packets to output file " << output_file << std::endl;

    return EXIT_SUCCESS;
}
