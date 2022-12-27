#  Script designed to remove PTR records from an input PCAP of DNS
#  Removes the PTR queries and reverses them to reconstruct the IP address
#  The resulting PTR records can be used as input for Grey Space analysis.
#  Input = PCAP or PCAPNG, Output = Text file

from scapy.all import *
from argparse import ArgumentParser


def main():
    parser = ArgumentParser()
    parser.add_argument('-f', '--file', help='Path to input PCAP file')
    parser.add_argument('-o', '--output', help='Path to output file')
    args = parser.parse_args()
    file_name = args.file
    caps = PcapNgReader(file_name)
    out_file = args.output
    output = []

    for packet in caps:
        if not packet.haslayer("DNS"):
            continue
        dns_layer = packet.getlayer("DNS")
        if dns_layer.qd.qtype == 12:
            query_name = (str(dns_layer.qd.qname))[2:]
            query_name = query_name.split(".")
            output_address = query_name[3] + "." +  query_name[2] + "." + query_name[1] + "." + query_name[0]
            if output_address in output:
                continue
            else:
                output.append(output_address)
    idx = len(output)
    with open (out_file, "w") as f:
        for i in range(0, idx):
            f.write(output[i] + "\n")


if __name__ == '__main__':
    main()
