#include <chrono>
#include <iomanip>
#include <iostream>
#include <locale>
#include <stdio.h>
#include <stdlib.h>

#include "heuristicthread.h"
#include "process_pcap.h"


#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"
#include "process_pcap.h"
#include "utilities.h"



HeuristicThread::HeuristicThread(int thread_id, std::string output_directory, bool generate_csv, bool verbose, int min_ip_matches, int min_mac_matches)
	: m_min_ip_matches(min_ip_matches), m_min_mac_matches(min_mac_matches), m_thread_id(thread_id), m_output_directory(output_directory), 
      m_generate_csv(generate_csv), m_verbose(verbose)
{
    m_working.store(false);
    m_running.store(false);
    m_abort_requested.store(false);
    m_extra_heuristic_name = "anomaly";

    m_ip_types.readProtocols("ip_protocols.txt", false, 10);
    m_ip_types.readProtocolPorts("ip_protocol_ports.txt");

    m_ethernet_types.read("eth_types.txt");
    m_dest_mac_to_ignore.read_file("destination_macs.txt");
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

        create_heuristic_anomaly_csv(m_dest_mac_to_ignore, m_mac_addresses, m_ip_addresses, m_ethernet_types, m_ip_types, pcap_filename, m_output_directory, m_extra_heuristic_name, m_verbose, m_generate_csv, &m_abort_requested, m_min_ip_matches, m_min_mac_matches);
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

