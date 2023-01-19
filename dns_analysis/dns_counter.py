#  Creates a list of Answers (A/AAAA) and PTR records
#  Removes the PTR queries and reverses them to reconstruct the IP address
#  Includes Query, Answer and Time to Live for all responses
#  Input = PCAP or PCAPNG, Output = PTR CSV, Answers CSV and Exceptions PCAP

from scapy.all import *
from argparse import ArgumentParser

def main():
    parser = ArgumentParser()
    parser.add_argument('-f', '--file', help='Path to input PCAP file')
    parser.add_argument("-s", "--status", help="Number of records to count before providing a status report", default=-1)
    args = parser.parse_args()
    status = int(args.status)
    file_name = args.file
    caps = PcapNgReader(file_name)
    if file_name[-6:] == "pcapng":
        base_name = file_name[:-7]
    if file_name[-4:] == "pcap":
        base_name = file_name[:-5]
    answers_out_file = base_name + "-answers.csv"

    answer_hosts = {}
    answer_output = {}
    pkt_idx = 1

    for packet in caps:
        #Filters for the DNS layer in the capture
        if not packet.haslayer("DNS"):
            continue
        dns_layer = packet.getlayer("DNS")
        dest = (packet.getlayer("IP")).dst
        # Targets A (IPv4, 1), AAAA (IPv6, 28), and PTR (12) queries
        targeted_types = [1,12,28]

        if not dns_layer.qr == 1:
            continue
        an_idx = dns_layer.ancount
        dest = (packet.getlayer("IP")).dst
        for i in range(0, an_idx):
            query_type = dns_layer.an[i].type
            ttl = dns_layer.an[i].ttl
            if query_type in targeted_types:
                rr_name = str(dns_layer.an[i].rrname)[2:].replace("'","")
                r_data = str(dns_layer.an[i].rdata)[2:].replace("'","")
                if query_type == 12:
                    rr_name = rr_name.split(".")
                    rr_name = rr_name[3] + "." + rr_name[2] + "." + rr_name[1] + "." + rr_name[0] 
                temp_output = (rr_name, r_data, query_type, ttl)
                if temp_output in answer_output:
                    count = answer_output[temp_output]
                    count += 1
                    answer_output[temp_output] = count
                    if not (dest in answer_hosts[temp_output]):
                        answer_hosts[temp_output] = answer_hosts[temp_output] + ";" + dest
                else:
                    answer_output[temp_output] = 1
                    answer_hosts[temp_output] = dest

        pkt_idx += 1
        if (pkt_idx % status == 0) and (status > 0):
            print("Processed " + str(pkt_idx) + "  packets.\n")
            
    answer_keys = list(answer_output.keys())
    idx = len(answer_keys)
    with open(answers_out_file, "w") as f:
        f.write("RR_Name,RData,Query_Type,TTL,Count,Host_List\n")
        for i in range(0, idx):
            count = answer_output[answer_keys[i]]
            f.write(str(answer_keys[i][0]) + ",")
            f.write(str(answer_keys[i][1]) + ",")
            f.write(str(answer_keys[i][2]) + ",")
            f.write(str(answer_keys[i][3]) + ",")
            f.write(str(count) + ",")
            host_list = str(answer_hosts[answer_keys[i]])
            f.write(host_list + "\n")

    
if __name__ == '__main__':
    main()
