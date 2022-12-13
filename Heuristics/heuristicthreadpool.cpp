#include "heuristicthreadpool.h"
#include "heuristicthread.h"
#include "utilities.h"


HeuristicThreadPool::HeuristicThreadPool(std::string output_directory, int numThreads, bool generate_csv, bool verbose) : 
	m_num_threads(numThreads), m_output_directory(output_directory)
{
	m_abort_requested.store(false);
	m_num_running.store(0);

	m_threads.reserve(m_num_threads);

	// TODO: Create actual threads!
	// Or do I create them when they are needed? 
	for (int i=0; i<m_num_threads; ++i) {
		HeuristicThread* htp = new HeuristicThread(i, m_output_directory, generate_csv, verbose);
		m_threads.push_back(htp);
		m_waiting_threads.insert(i);
	}
}

HeuristicThreadPool::~HeuristicThreadPool()
{
    abortAndJoin();
    // TODO: Put in a wait until things are no longer running!
	for (HeuristicThread* thread : m_threads) {
    	if (thread != nullptr) {
    		delete thread;
    	}
    }
}

void HeuristicThreadPool::abortAndJoin()
{
    if (!m_abort_requested.load()) {
        m_abort_requested.store(true);
        for (HeuristicThread* thread : m_threads) {
        	if (thread != nullptr) {
        		thread->abortAndJoin();
        	}
        }
    }
}

bool HeuristicThreadPool::run()
{
    try 
    {
        //m_thread = std::thread(&HeuristicThreadPool::runFunc, this);
        std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
        while (!m_pcap_name.empty() && !m_waiting_threads.empty()) {
        	std::set<int>::iterator it = m_waiting_threads.begin();
        	if (it != m_waiting_threads.end()) {
        		int thread_id = *it;
        		m_waiting_threads.erase(it);
            	std::string nextFile = m_pcap_name.front();
	        	m_pcap_name.pop();
	        	m_threads[thread_id]->setNextPcap(nextFile);
	        	m_num_running++;
    		}
        }
    }
    catch(...) 
    {
        return false;
    }

    return true;
}

bool HeuristicThreadPool::addFile(const std::string& filename) {
	if (m_abort_requested.load()) {
		std::cout << "Not adding file, abort requested." << std::endl;
		return false;
	}
	if (filename.empty())
		return false;
	try {
        std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
        m_pcap_name.push(filename);
    }
    catch(...) {
    	// TODO: Print error
        return false;
    }
	return true;
}

bool HeuristicThreadPool::addFile(const std::vector<std::string>& filenames) {
	if (m_abort_requested.load()) {
		std::cout << "Not adding file(s), abort requested." << std::endl;
		return false;
	}
	if (filenames.empty())
		return false;
	try {
        std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
        for (std::string s : filenames) {
        	if (!s.empty())
        		m_pcap_name.push(s);
        }
    }
    catch(...) {
    	// TODO: Print error
        return false;
    }
	return true;
}

void HeuristicThreadPool::aborting(int threadID) {
	try {
        std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
   		m_waiting_threads.insert(threadID);
    	m_num_running--;
    }
    catch(...) {
    	// TODO: Print error
    }
}

std::string HeuristicThreadPool::finishedProcessing(int threadID) {
	try {
        std::lock_guard guard(m_pcap_name_mutex); // CTAD, C++17
        if (m_pcap_name.empty()) {
        	// Move to available list.
       		m_waiting_threads.insert(threadID);
        	m_num_running--;
        } else {
        	std::string nextFile = m_pcap_name.front();
        	m_pcap_name.pop();
        	return nextFile;
    	}
    }
    catch(...) {
    	// TODO: Print error
    }
    return "";
}

void HeuristicThreadPool::setOutputDirectory(const std::string& output_directory) {
	m_output_directory = output_directory;
	if (!m_output_directory.empty() && !hasEnding(m_output_directory, "/", false)) {
		m_output_directory.append("/");
	}
	try {
        std::lock_guard guard(m_pcap_name_mutex);
    	for (HeuristicThread* thread : m_threads) {
        	if (thread != nullptr) {
        		thread->setOutputDirectory(m_output_directory);
        	}
        }
    }
    catch(...) {
    	// TODO: Print error
    }
}

void HeuristicThreadPool::setExtraHeuristicName(const std::string& name) {
	for (HeuristicThread* thread : m_threads) {
    	if (thread != nullptr) {
    		thread->setExtraHeuristicName(name);
    	}
    }
}

void HeuristicThreadPool::processDirectory(std::string dir_path, std::string spec, bool isRegEx) {
	std::vector<std::string> files = readDirectory(dir_path, spec, isRegEx, true, false);
	if (files.empty()) {
		std::cout << "No files matching file spec " << spec << " found in directory " << dir_path << std::endl;
	} else {
		std::cout << "Found " << files.size() << " files matching file spec " << spec << " in directory " << dir_path << std::endl;
		addFile(files);
		run();
	}
}
