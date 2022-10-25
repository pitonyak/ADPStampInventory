#include <cerrno>
#include <cstring>
#include <fstream>
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
 * @brief Reads a 6-byte sequence from the given file path which will be used to match against the vendor ids in ISAKMP
 * packets.
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
 * @brief Searches the given pcap file for ISAKMP packets that contain vendor ID payloads with the specified matching
 * set of bytes and records the source and destination IP addresses associated with these packets.
 *
 * @param[in,out] pcap_file The pcap file handle for which to search for ISAKMP packets.
 * @param[in] vendor_match Vector of bytes to match in the vendor id payloads of ISAKMP packets.
 * @param[in,out] address_set The object in which to record source and destination IP addresses for matching ISAKMP
 * packets.
 * @param[in] verbose_output_enabled Flag indicating whether verbose output is enabled for the program.
 */
void find_address_pairs(pcap_handle &pcap_file, const std::vector<uint8_t> &vendor_match, IPAddressPairSet &address_set,
                        bool verbose_output_enabled)
{
    struct pcap_pkthdr *packet_header;
    const uint8_t *packet_data;

    while (true)
    {
        int pcap_result = pcap_next_ex(pcap_file.get(), &packet_header, &packet_data);
        if (pcap_result == PCAP_ERROR_BREAK)
        {
            if (verbose_output_enabled)
            {
                std::cout << "Done reading packets from input pcap file.\n";
            }
            return;
        }
        else if (pcap_result == PCAP_ERROR)
        {
            pcap_perror(pcap_file.get(), "Error encountered while reading packets from pcap");
            continue;
        }

        const auto *ethernet_header = reinterpret_cast<const struct ether_header *>(packet_data);
        const uint16_t ethernet_type = ntohs(ethernet_header->ether_type);
        if (ethernet_type == ETHERTYPE_IP)
        {
            const auto *ip_header = reinterpret_cast<const struct ip *>(packet_data + sizeof(struct ether_header));
            if (ip_header->ip_p != IPPROTO_UDP)
            {
                // TODO: Investigate whether ISAKMP traffic can be sent over TCP and handle that.
                continue;
            }

            // TODO: Handle fragmented packets. ASan may raise errors if the pcap has such packets.

            const size_t offset_to_ip_payload = sizeof(struct ether_header) + ip_header->ip_hl * sizeof(uint32_t);
            const auto *udp_header = reinterpret_cast<const struct udphdr *>(packet_data + offset_to_ip_payload);

            uint16_t source_port = ntohs(udp_header->uh_sport);
            uint16_t destination_port = ntohs(udp_header->uh_dport);
            constexpr uint16_t isakmp_port = 500;
            if (source_port != isakmp_port && destination_port != isakmp_port)
            {
                // TODO: Check port 4500 for UDP-encapsulated packets.
                continue;
            }

            const size_t offset_to_udp_payload = offset_to_ip_payload + sizeof(struct udphdr);
            const auto *isakmp_header =
                reinterpret_cast<const struct isakmp_header *>(packet_data + offset_to_udp_payload);

            if (isakmp_header->flags & isakmp_flags::IKE_Encrypt)
            {
                // Skip packets with encrypted payloads for IKEv1.
                continue;
            }

            const size_t offset_to_isakmp_payload = offset_to_udp_payload + sizeof(struct isakmp_header);
            const auto *current_payload =
                reinterpret_cast<const struct isakmp_generic_payload_header *>(packet_data + offset_to_isakmp_payload);
            isakmp_payload_type current_payload_type = isakmp_header->next_payload;

            while (current_payload_type != isakmp_payload_type::None &&
                   current_payload_type != isakmp_payload_type::IKE2_Encrypted)
            {
                if (current_payload_type == isakmp_payload_type::IKE_VendorId ||
                    current_payload_type == isakmp_payload_type::IKE2_VendorId)
                {
                    const auto *vendor_id = reinterpret_cast<const uint8_t *>(current_payload) +
                                            sizeof(struct isakmp_generic_payload_header);
                    if (memcmp(vendor_id, vendor_match.data(), vendor_match.size()) == 0)
                    {
                        address_set.add_pair(ip_header->ip_src, ip_header->ip_dst);
                        break;
                    }
                }

                // Get next payload and its type.
                current_payload_type = current_payload->next_payload;
                current_payload = reinterpret_cast<const struct isakmp_generic_payload_header *>(
                    reinterpret_cast<const uint8_t *>(current_payload) + ntohs(current_payload->length));
            }
        }
        else if (ethernet_type == ETHERTYPE_IPV6)
        {
            // TODO: Handle IPv6.
            if (verbose_output_enabled)
            {
                std::cout << "Ignoring IPv6 packet\n";
            }
        }
        else if (verbose_output_enabled)
        {
            if (verbose_output_enabled)
            {
                std::cout << "Ignoring packet of type " << ethernet_type << "\n";
            }
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
                std::cout << "Done reading packets from input pcap file.\n";
            }
            break;
        }

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
            }
        }
        else if (ethernet_type == ETHERTYPE_IPV6)
        {
            // TODO: Handle IPv6.
            if (verbose_output_enabled)
            {
                std::cout << "Ignoring IPv6 packet\n";
            }
        }
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
    out << "Usage: " << program_name << " [options] -r <> -w <outfile>\n\n";
    out << "  -r, --read-file=FILE    set input pcap filename\n";
    out << "  -w, --write-file=FILE   set output pcap filename\n";
    out << "  -m, --match-file=FILE   set filename to read vendor id bytes to match against ISAKMP packets\n";
    out << "  -v, --verbose           display verbose output\n";
    out << "  -h, --help              display this help and exit" << std::endl;
}

int main(int argc, char **argv)
{
    // Define set of options that can be passed on the command line for this program.
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"match-file", required_argument, 0, 'm'},
        {"read-file", required_argument, 0, 'r'},
        {"verbose", no_argument, 0, 'v'},
        {"write-file", required_argument, 0, 'w'},
        {nullptr, 0, nullptr, 0},
    };

    std::string input_file, output_file, match_file;
    bool verbose_output_enabled = false;
    int option, option_index;
    while ((option = getopt_long(argc, argv, "hr:m:vw:", long_options, &option_index)) != -1)
    {
        switch (option)
        {
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
        case 'v':
            verbose_output_enabled = true;
            break;
        case 'w':
            output_file = optarg;
            break;
        case '?':
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
        std::cout << "Opened pcap file at " << input_file << std::endl;
    }

    // Ensure that the pcap file only has Ethernet packets
    if (pcap_datalink(pcap_file.get()) != DLT_EN10MB)
    {
        std::cerr << "PCAP file " << input_file << " is not an Ethernet capture." << std::endl;
        return EXIT_FAILURE;
    }

    // Find source/destination address pairs that are sending ISAKMP packets with matching vendor id.
    IPAddressPairSet address_set;
    find_address_pairs(pcap_file, vendor_match, address_set, verbose_output_enabled);

    std::cout << "Found " << address_set.size() << " unique IP address pairs from matching ISAKMP packets.\n";

    // Bail out early if no matching ISAKMP packets were found in the input pcap file.
    if (address_set.size() == 0)
    {
        std::cout << "Skipped writing filtered packets to output file becausse no ISAKMP packets matched.\n";
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
    std::cout << "Wrote filtered packets to " << output_file << std::endl;

    return EXIT_SUCCESS;
}
