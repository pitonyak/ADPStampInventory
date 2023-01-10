# Green Team Toolbox DNS Analysis

This directory contains tools for DNS Analysis

The base files (by name) are as follows: <br/>

| file / directory  | Description  |
|-------------------|--------------|
| dns_counter.py    | Count PTR records and DNS A/AAAA answers. |

The individual tools are listed below.
 

## dns_counter.py

Original script by Beau Ward. 

This script uses the ArgumentParser and scapy.all.

This script counts PTR records and DNS A/AAAA answers
removing the PTR queries and reverses them to reconstruct the IP address.
The script also creates a list of A/AAAA responses.
Exception packets are placed in an anomaly PCAP file.

A CSV file is generated that can be used as input for Grey Space analysis.

Input = PCAP or PCAPNG, Output = PTR CSV, Answers CSV and Exceptions PCAP

### Usage

| short | long      | Description  |
|-------------------|--------------|
| -f    | --file    | Path to input CPAP file |
| -s    | --status  | Number of records to count before providing a status report |

The output CSV file is take from the base file name obtained by removing
 the file extension, which is expcted to be pcapng or pcap.


Three files are generated: 

1. The ptr output file is basename + '-ptr.csv"

2. The answers output file is basename + 'answers.csv'

3. The anomaly output file is basename + '-anomalies.pcap'

### -answers.csv file

The following columns are generated: 

| Column       | Description  |
|--------------|--------------|
| DNS_Query    | TBD |
| DNS_Response | TBD |


### -ptr.csv file

TODO: Note that this file contains a trailing comma in the header which should
probably be removed. 

The following columns are generated: 

| Column         | Description  |
|----------------|--------------|
|  Pointer_Query | TBD |
|  Count         | TBD |


<br/>
