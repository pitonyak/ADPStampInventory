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