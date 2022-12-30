#  Script designed to count PTR records and DNS A/AAAA answers
#  Removes the PTR queries and reverses them to reconstruct the IP address
#  Creates a list of A/AAAA responses
#  Any exceptions are placed in an anomaly PCAP file
#  The output CSV files could be used as input for Grey Space analysis.
#  Input = PCAP or PCAPNG, Output = PTR CSV, Answers CSV and Exceptions PCAP

from scapy.all import *
from argparse import ArgumentParser

def main():
    parser = ArgumentParser()
    parser.add_argument('-f', '--file', help='Path to input PCAP file')
    parser.add_argument("-s", "--status", help="Number of records to count before providing a status report", default=0)
    args = parser.parse_args()
    status = int(args.status)
    file_name = args.file
    caps = PcapNgReader(file_name)
    if file_name[-6:] == "pcapng":
        base_name = file_name[:-7]
    if file_name[-4:] == "pcap":
        base_name = file_name[:-5]
    ptr_out_file = base_name + "-ptr.csv"
    answers_out_file = base_name + "-answers.csv"
    anomaly_out_file = base_name + "-anomalies.pcap"

    ptr_output = []
    ptr_counts = {}
    answer_output = []
    anomaly_output = []
    pkt_idx = 1

    for packet in caps:
        if not packet.haslayer("DNS"):
            continue
        dns_layer = packet.getlayer("DNS")
        
        #wrap code in an exception to avoid problems created by TCP fragments
        try:
            # code to identify PTR records (query type 12)
            # and reverse PTR string to get the queried IP
            if dns_layer.qd.qtype == 12:
                query_name = (str(dns_layer.qd.qname))[2:]
                query_name = query_name.split(".")
                output_address = query_name[3] + "." +  query_name[2] + "." + query_name[1] + "." + query_name[0]
                if output_address in ptr_output:
                    current = ptr_counts[output_address]
                    current += 1
                    ptr_counts[output_address] = current
                else:
                    ptr_output.append(output_address)
                    ptr_counts[output_address] = 1
            # code to identify DNS responses 
            # and get both the queried host name and IP
            if (dns_layer.ancount > 0):
                an_idx = dns_layer.ancount - 1
                for i in range(an_idx, 0, -1):
                    # Select A (1) or AAAA (28) responses
                    if dns_layer.an[i].type == 1 or dns_layer.an[i].type == 28:
                        rr_name = str(dns_layer.an[i].rrname)[2:].replace("'","")
                        r_data = str(dns_layer.an[i].rdata)
                        answer_output.append((rr_name, r_data))
        except:
            anomaly_output.append(packet)

        pkt_idx += 1
        if (pkt_idx % status == 0) and (status > 0):
            print("Processed " + str(pkt_idx) + "  packets.\n")

    idx = len(ptr_output)
    with open (ptr_out_file, "w") as f:
        f.write(("Pointer_Query,Count,\n"))
        for i in range(0, idx):
            f.write(ptr_output[i] + "," + str(ptr_counts[ptr_output[i]]) + "\n")
    idx = len(answer_output)
    with open(answers_out_file, "w") as f:
        f.write("DNS_Query,DNS_Response\n")
        for i in range(0, idx):
            f.write(answer_output[i][0] + "," + answer_output[i][1] + "\n")
    wrpcap(anomaly_out_file, anomaly_output)

if __name__ == '__main__':
    main()
