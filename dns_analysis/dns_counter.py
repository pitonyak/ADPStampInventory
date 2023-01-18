#  Script designed to count PTR records and DNS A/AAAA answers
#  Removes the PTR queries and reverses them to reconstruct the IP address
#  Creates a list of Answers (A/AAAA) and PTR records
#  Includes hosts and counts of queries
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
    ptr_out_file = base_name + "-ptr.csv"
    answers_out_file = base_name + "-answers.csv"
    anomaly_out_file = base_name + "-anomalies.pcap"

    ptr_output = {}
    ptr_hosts = {}
    answer_hosts = {}
    answer_output = {}
    anomaly_output = []
    pkt_idx = 1

    for packet in caps:
        #Filters for the DNS layer in the capture
        if not packet.haslayer("DNS"):
            continue
        dns_layer = packet.getlayer("DNS")
        dst = [(packet.getlayer("IP")).dst]

        try:
            if not dns_layer.qr == 1:
                continue
            an_idx = dns_layer.ancount
            dst = [(packet.getlayer("IP")).dst]
            for i in range(0, an_idx):
                query_type = dns_layer.an[i].type
                ttl = dns_layer.an[i].ttl
                if query_type == 1 or query_type == 28:
                    rr_name = str(dns_layer.an[i].rrname)[2:].replace("'","")
                    r_data = str(dns_layer.an[i].rdata)
                    temp_output = (rr_name, r_data, ttl)
                    if temp_output in answer_output:
                        count = answer_output[temp_output]
                        count += 1
                        answer_output[temp_output] = count
                        answer_hosts[temp_output] = (answer_hosts[temp_output]).insert(0,dst)
                    else:
                        answer_output[temp_output] = 1
                        answer_hosts[temp_output] = dst
                       


                if query_type == 12:
                    temp_rr_name = str(dns_layer.an[i].rrname)[2:].replace("'","")
                    ptr_r_data = str(dns_layer.an[i].rdata)[2:].replace("'","")
                    temp_rr_name = temp_rr_name.split(".")
                    ptr_rr_name = temp_rr_name[3] + "." + temp_rr_name[3] + "." + temp_rr_name[3] + "." + temp_rr_name[0]
                    temp_output = (ptr_rr_name, ptr_r_data, ttl)
                    if temp_output in ptr_output:
                        count = ptr_output[temp_output]
                        count += 1
                        ptr_output[temp_output] = count
                        ptr_hosts[temp_output] = (ptr_hosts[temp_output]).insert(0,dst)
                    else:
                        ptr_output[temp_output] = 1
                        ptr_hosts[temp_output] = dst
        except:
            anomaly_output.append(packet)

        pkt_idx += 1
        if (pkt_idx % status == 0) and (status > 0):
            print("Processed " + str(pkt_idx) + "  packets.\n")

    ptr_keys = list(ptr_output.keys())
    idx = len(ptr_keys)
    with open (ptr_out_file, "w") as f:
        f.write(("Pointer_Query,Answer,TTL,Count,Host_List\n"))
        for i in range(0, idx):
            count = ptr_output[ptr_keys[i]]
            f.write(str(ptr_keys[i][0]) + ",")
            f.write(str(ptr_keys[i][1]) + ",")
            f.write(str(ptr_keys[i][2]) + ",")
            f.write(str(count) + ",")
            host_list = str(ptr_hosts[ptr_keys[i]]).replace(",",";")
            f.write(host_list + "\n")
            
            
    answer_keys = list(answer_output.keys())
    idx2 = len(answer_keys)
    with open(answers_out_file, "w") as f:
        f.write("DNS_Query,DNS_Response,TTL,Count,Host_List\n")
        for i in range(0, idx2):
            count = answer_output[answer_keys[i]]
            f.write(str(answer_keys[i][0]) + ",")
            f.write(str(answer_keys[i][1]) + ",")
            f.write(str(answer_keys[i][2]) + ",")
            f.write(str(count) + ",")
            host_list = str(answer_hosts[answer_keys[i]]).replace(",",";")

    if len(anomaly_output) > 0:
        wrpcap(anomaly_out_file, anomaly_output)
    
if __name__ == '__main__':
    main()
