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

class HeuristicThreadPool;

//**************************************************************************
//! A thread that will run the heuristic.
/*!
 * 
 * A thread that will run the heuristic.
 * 
 * 
 * 
 *
 ***************************************************************************/

class HeuristicThread
{
public:
    //**************************************************************************
    //! Constructor
    /*!
     * 
     * Sets and then reads specific files needed to process a PCAP;
     * for example, the IP types stored in ip_protocols.txt and 
     * ip_protocol_ports.txt, the supported ethernet types stored in 
     * eth_types.txt, and the destination MACS to ignore in destination_macs.txt.
     * 
     * All of these files are read, which means that an exception can be thrown out of 
     * the constructor. Consider moving this code elsewhere so as to protect the
     * constructor from exceptions.
     * 
     * The extra heuristic name is hard coded to be "anomaly", but may be modified 
     * by setting this in the heuristic pool object, which then sets it for each thread.
     * 
     * On exit, this object is not running or working and abort has not been requested.
     * 
     * \param [in] thread_id Thread index (or id) in the thread pool object.
     *
     * \param [in] output_directory Directory that will (or does) contain the MAC and IP files. If empty, stored in the same directory as the PCAP file.
     *
     * \param [in] generateCSV If True, generate a CSV file, otherwise do not create a CSV file.
     *
     * \param [in] verbose Should information be printed while processing a file.
     *
     * \param [in] dest_mac_to_ignore All traffic to this MAC address is ignored while creating the anomaly file.
     *
     * \param [in] min_ip_matches If zero, finding at least 1 IP causes the packet to be written to be dumped to the anomaly file.
     *                            If > zero, must have at least that many unique matches.
     *                            If < zero, do not search for an IP.
     * 
     * \param [in] min_mac_matches If zero, finding at least 1 MAC causes the packet to be written to be dumped to the anomaly file.
     *                             If > zero, must have at least that many unique matches.
     *                             If < zero, do not search for a MAC.
     * 
     ***************************************************************************///
    HeuristicThread(HeuristicThreadPool* parent, int thread_id = 0, std::string output_directory="", bool generate_csv=true, bool verbose=false, int min_ip_matches=2, int min_mac_matches=2);

    /*! Destructor. Currently simple, calling abort and join. */
    ~HeuristicThread();

    //**************************************************************************
    //! Create and start running the thread.
    /*!
     * 
     * If not already running, create a new thread, which should call the runFunc.
     * Is is the runFunc that will set this to the running state.
     * 
     * This is a little dangerous in that someone could call run multiple times
     * before the runFunc is actually called. 
     * 
     * \returns True if this starts running. 
     *
     ***************************************************************************///
    bool run();

    //**************************************************************************
    //! Use a mutex and sets the pcap filename.
    /*!
     * 
     * Called from the thread pool when a new file is expected to be processed.
     * 
     * \param [in] pcap_filename Next PCAP filename to process. 
     * 
     ***************************************************************************///
    void setNextPcap(std::string pcap_filename);

    /*! Is this thread running? */
    bool running() const 
    {
        return m_running.load();
    }

    /*! Asks the thread to stop running (abort and join). */
    void stop()
    {
        abortAndJoin();
    }

    /*! Asks the thread to stop running (abort and join). */
    void abortAndJoin();

    std::string getOutputDirectory() const { return m_output_directory; }
    void setOutputDirectory(const std::string& output_directory);
    std::string getExtraHeuristicName() const { return m_extra_heuristic_name; }
    void setExtraHeuristicName(const std::string& name) { m_extra_heuristic_name = name; }

    int getMinIpMatches() const { return m_min_ip_matches; }
    void setMinIpMatches(int x) { m_min_ip_matches = x; }

    int getMinMacMatches() const { return m_min_mac_matches; }
    void setMinMacMatches(int x) { m_min_mac_matches = x; }

private:
    //**************************************************************************
    //! Main outer loop run while abort has not been requested.
    /*!
     * 
     * This loop checks to see if the pcap file name has been set. 
     * If the file has been set, store it for processing, clear it, and then 
     * start processing the file.
     * 
     * Actual processing is done by doWork().
     * 
     ***************************************************************************///
    void runFunc();

    //**************************************************************************
    //! Process a file.
    /*!
     * 
     * Called from runFunc().
     * 
     * \param [in] pcap_filename Next PCAP filename to process. 
     * 
     ***************************************************************************///
    void doWork(std::string pcap_filename);

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

    int m_min_ip_matches;
    int m_min_mac_matches;

    /*! Thread index in the thread pool object. */
    int m_thread_id;

    /*! Directory that will (or does) contain the MAC and IP files. If empty, stored in the same directory as the PCAP file. */
    std::string m_output_directory;

    /*! If True, generate a CSV file, otherwise do not create a CSV file. */
    bool m_generate_csv;

    /*! Should extra debug information be printed while processing a file. */
    bool m_verbose;

    /*! Filename of the PCAP file to be processed next. This filename is cleared when work is started. */
    std::string m_pcap_name;

    /*! May be used in creating the filenames used for the MAC and IP addresses. */
    std::string m_extra_heuristic_name;

    /*! Associated to the thread in the run function. */
    std::thread m_thread;

    /*! If true, the thread is actively running, which is not the same as working. But run has been called so the thread can be aborted. */
    std::atomic_bool m_running;

    /*! If true, the thread is actively processing a file  */
    std::atomic_bool m_working;

    /*! Used to ask a thread to cancel processing. */
    std::atomic_bool m_abort_requested;

    /*! TODO: ???? */
    std::recursive_mutex m_pcap_name_mutex;

    /*! This will contain the MAC addresses contained in the PCAP file. If the file already exist, it is simply read. */
    MacAddresses m_mac_addresses;

    /*! All traffic to this MAC address is ignored while creating the anomaly file. */
    MacAddresses m_dest_mac_to_ignore;

    /*! This will contain the MAC addresses contained in the PCAP file. If the file already exist, it is simply read. */
    IpAddresses m_ip_addresses;

    /*! Contains the list of IP Types and ports, are they valid, are other MAC or IP addresses expected in the payload. */
    IPTypes m_ip_types;

    /*! Contains the list of Ethernet Types, are they valid, are other MAC or IP addresses expected in the payload. */
    EthernetTypes m_ethernet_types;

    HeuristicThreadPool* m_parent;
};


#endif