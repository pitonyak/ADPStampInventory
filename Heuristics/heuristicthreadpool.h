#ifndef HEURISTIC_THREAD_POOL_H
#define HEURISTIC_THREAD_POOL_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <set>
#include <thread>

class HeuristicThread;

class HeuristicThreadPool
{
public:
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
     * \param [in] output_directory - Where the output files stored?
     * 
     * \param [in] numThreads - Number of threads to create.
     * 
     ***************************************************************************/
    HeuristicThreadPool(std::string output_directory="", int numThreads=5, bool generate_csv=true, bool verbose=false);

    ~HeuristicThreadPool();

    // To be called, once the should start.
    bool run();

    int numRunning() const 
    {
        return m_num_running.load();
    }

    void stop()
    {
        abortAndJoin();
    }

    void abortAndJoin();

    bool addFile(const std::string& filename);
    bool addFile(const std::vector<std::string>& filenames);

    std::string finishedProcessing(int threadID);
    void aborting(int threadID);

    std::string getOutputDirectory() const { return m_output_directory; }

    //**************************************************************************
    //! Set the output directory in this object and every thread.
    /*!
     * 
     * \param [in] output_directory - Where output files are stored.
     * 
     ***************************************************************************/
    void setOutputDirectory(const std::string& output_directory);

    void setExtraHeuristicName(const std::string& name);

    void processDirectory(std::string dir_path, std::string spec="*.pcap", bool isRegEx=false);



private:
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
    HeuristicThreadPool(const HeuristicThreadPool& x);

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
    HeuristicThreadPool(HeuristicThreadPool&& x);

    int m_num_threads;
    std::string m_output_directory;
    std::vector<HeuristicThread*> m_threads;
    std::set<int> m_waiting_threads;
    std::atomic_int m_num_running;
    std::atomic_bool m_abort_requested;

    std::recursive_mutex m_pcap_name_mutex;
    std::queue<std::string> m_pcap_name;
};


#endif