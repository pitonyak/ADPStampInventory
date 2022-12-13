#ifndef HEURISTIC_THREAD_H
#define HEURISTIC_THREAD_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "ethtype.h"
#include "ipaddresses.h"
#include "iptype.h"
#include "macaddresses.h"

class HeuristicThread
{
public:
    /*! Constructor */
    HeuristicThread(int thread_id = 0, std::string output_directory="", bool generate_csv=true, bool verbose=false);

    ~HeuristicThread();

    bool run();
    void setNextPcap(std::string pcap_filename);
    void doWork(std::string pcap_filename);

    bool running() const 
    {
        return m_running.load();
    }

    void stop()
    {
        abortAndJoin();
    }

    void abortAndJoin();

    std::string getOutputDirectory() const { return m_output_directory; }
    void setOutputDirectory(const std::string& output_directory);

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
    int write_ip_and_mac_from_pcap(const std::string& pcap_filename, const std::string& out_mac_fname, const std::string& out_ip_fname);

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
    void read_create_mac_ip_files(const std::string& pcap_filename);
        
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
    int create_heuristic_anomaly_csv(const EthernetTypes& ethernet_types, const IPTypes& ip_types, const std::string& pcap_filename, bool verbose, bool generateCSV);


    std::string getExtraHeuristicName() const { return m_extra_heuristic_name; }
    void setExtraHeuristicName(const std::string& name) { m_extra_heuristic_name = name; }

private:
    void runFunc();

    //**************************************************************************
    //! Copy Constructor.
    /*!
     * This object is not implemented. I have no particular use for one and
     * this is an attempt to make it more difficult to call. 
     * 
     * Calling from outside the class is a compile error. 
     * 
     * Calling from inside the class is a link error. 
     * 
     * \param [in] x - Object to copy.
     * 
     ***************************************************************************/
    HeuristicThread(const HeuristicThread& x);

    //**************************************************************************
    //! Move Constructor.
    /*!
     * This one I should probably implement. 
     * First lets see if it is used / needed. The default might be fine. 
     * 
     * Calling from outside the class is a compile error. 
     * 
     * Calling from inside the class is a link error. 
     * 
     * \param [in] x - Object to move.
     * 
     ***************************************************************************/
    HeuristicThread(HeuristicThread&& x);

    int m_thread_id;
    std::string m_output_directory;
    bool m_generate_csv;
    bool m_verbose;
    std::string m_pcap_name;
    std::string m_extra_heuristic_name;
    std::thread m_thread;
    std::atomic_bool m_running;
    std::atomic_bool m_working;
    std::atomic_bool m_abort_requested;
    std::recursive_mutex m_pcap_name_mutex;
    MacAddresses m_mac_addresses;
    MacAddresses m_dest_mac_to_ignore;
    IpAddresses m_ip_addresses;
    IPTypes m_ip_types;
    EthernetTypes m_ethernet_types;
};


#endif