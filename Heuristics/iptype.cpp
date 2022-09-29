
#include <queue>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include "iptype.h"
#include "utilities.h"

IPType::IPType() : 	m_iPType(false), m_valid(false), m_dupIP(false), m_dupMAC(false)
{
}

IPType::IPType(const IPType& ipt)
{
	if (this != &ipt)
    {
    	m_iPType = ipt.m_iPType;
		m_valid = ipt.m_valid;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
}

IPType::~IPType() {}

const IPType& IPType::operator=(const IPType& ipt)
{
	if (this != &ipt)
    {
    	m_iPType = ipt.m_iPType;
		m_valid = ipt.m_valid;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
    return *this;
}

std::ostream& IPType::print(std::ostream& x) const
{
	x << std::setw(4) << m_iPType << " " << m_valid << " " << m_dupIP << "  " << m_dupMAC << " " << std::setw(3) << m_port << "   " << m_description;
	return x;
}

IPTypes::IPTypes()
{
}

IPTypes::~IPTypes()
{
	for (auto it : m_ipTypes) {
		for (auto it2 : *(it.second)) {
			delete it2.second;
		}
		delete it.second;
	}
	m_ipTypes.clear();
}

void IPTypes::addType(const IPType& ipt)
{
	addType(new IPType(ipt), true);
}

void IPTypes::addType(IPType* ipt, bool ownit)
{
	if (ipt == nullptr)
		return;
	if (!ownit) {
		addType(*ipt);
		return;
	}


  // The first int is the protocol type. 
  // The second int is the port.
  // unordered_map<int, unordered_map<int, IPType *>* > m_ipTypes;
  // This object is a map of ports to types. 
	std::unordered_map<int, IPType *>* a_type_map = nullptr;

  // First check to see if we know about the protocol type.
  // a_type_map will piont to either the existing one or a new one is created if needed. 
	std::unordered_map<int, std::unordered_map<int, IPType *>* >::iterator it_top = m_ipTypes.find(ipt->m_iPType);
	a_type_map = (it_top == m_ipTypes.end()) ? new std::unordered_map<int, IPType *>() : it_top->second;

	if (it_top == m_ipTypes.end()) {
		m_ipTypes[ipt->m_iPType] = a_type_map;
	}

  // Now check to see if we know about the port. 
	std::unordered_map<int, IPType *>::iterator it_secondary = a_type_map->find(ipt->m_port);

	if (it_secondary != a_type_map->end()) {
    // The protocol type and port already exists, so simply over-write what we have already
    (*(*a_type_map)[ipt->m_port]) = *ipt;
    delete ipt;
	} else {
    (*a_type_map)[ipt->m_port] = ipt;
  }
}


bool IPTypes::read(const std::string& filename, int base)
{
  return readProtocols(filename, true, base);
}

bool IPTypes::readProtocolPorts(const std::string& filename) {
  int current_protocol = -1;
  if (!isPathExist(filename, true, false, true, false)) {
    std::cout << "File does not exist or cannot be read: " << filename << std::endl;
    return false;
  }

  std::ifstream file(filename);
  if(!file.is_open()){
    std::cout << "Failed to open file: " << filename << std::endl;
    return false;
  }

  int line_count = 0;
  const int num_int_cols = 4;
  const std::string protocolString = "protocol=";
  std::string line;
  std::queue<std::string> lineq;
  // Note that reading a file using a FILE* is 4 to 5 times faster.
  // Processing speed is not an issue since this is not done often and the file is small.
  while (std::getline(file, line)) {
    
    ++line_count;
    if (reduce_input_string(line)) {
      continue;
    }

    if (line.length() > 9) {
      std::string line_copy = line.substr(0, 9);
      std::transform(line_copy.begin(), line_copy.end(), line_copy.begin(), [](unsigned char c){ return std::tolower(c); });
      if (line_copy.compare(protocolString) == 0) {
        line_copy = line.substr(9);
        current_protocol = std::stoi(line_copy, nullptr, 10);
        //std::cout << "Processing IP protocol " << current_protocol << " for known ports in configuration file." << std::endl;
        continue;
      }
    }
    //
    // There are four integers followed by a string description.
    // ip protocol/type, OK/valid, IPs in payload, MACs in payload, Description
    // 
    if (parse_reduce_line(line, lineq, num_int_cols)) {
      continue;
    }
    //
    // A range is not allowed here!
    //
    if (current_protocol < 0) {
      std::cout << "Protocol not yet set while reading protocol ports at line " << line_count << std::endl;
      continue;
    }
    IPType* baseType = new IPType();
    baseType->m_iPType = current_protocol;
    baseType->m_port = std::stoi(lineq.front(), nullptr, 10);
    lineq.pop();
    baseType->m_valid = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_dupIP = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_dupMAC = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    if (!lineq.empty()) {
      baseType->m_description = lineq.front();
      lineq.pop();
    }
    addType(baseType, true);
  }
  file.close();
  return true;
}

bool IPTypes::readProtocols(const std::string& filename, bool hasPort, int base)
{
  if (!isPathExist(filename, true, false, true, false)) {
    std::cout << "File does not exist or cannot be read: " << filename << std::endl;
    return false;
  }

  std::ifstream file(filename);
  if(!file.is_open()){
    std::cout << "Failed to open file: " << filename << std::endl;
    return false;
  }

  int line_count = 0;
  std::string line;
  std::queue<std::string> lineq;
  int num_int_cols = hasPort ? 5 : 4;
  // Note that reading a file using a FILE* is 4 to 5 times faster.
  // Processing speed is not an issue since this is not done often and the file is small.
  while (std::getline(file, line)) {
    
    ++line_count;
    std::string original_line = line;
    //
    // There are four integers followed by a string description.
    // ip protocol/type, OK/valid, IPs in payload, MACs in payload, Description
    // 
    if (parse_reduce_line(line, lineq, num_int_cols)) {
      continue;
    }

    std::string token = lineq.front();
    int range_start = 0;
    int range_end = 0;
    lineq.pop();
    parse_range(token, range_start, range_end, base);

    IPType* baseType = new IPType();
    try {

      baseType->m_iPType = range_start;
      baseType->m_valid = std::stoi(lineq.front(), nullptr, 10) != 0;
      lineq.pop();
      baseType->m_dupIP = std::stoi(lineq.front(), nullptr, 10) != 0;
      lineq.pop();
      baseType->m_dupMAC = std::stoi(lineq.front(), nullptr, 10) != 0;
      lineq.pop();
      if (hasPort) {
        baseType->m_port = std::stoi(lineq.front(), nullptr, 10);
        lineq.pop();
      } else {
        baseType->m_port = -1;
      }
      if (!lineq.empty()) {
        baseType->m_description = lineq.front();
        lineq.pop();
      }
      addType(baseType, true);

    } catch (std::invalid_argument& e) {
      std::cout << "Exception processing line " << line_count  << " (" << original_line << ") of file " << filename << std::endl;
      if (e.what() != nullptr) {
        std::cout << e.what() << std::endl;
      }
      delete baseType;
      baseType = nullptr;
      range_end = range_start;
    }
  
    while (range_start < range_end) {
      ++range_start;
      baseType = new IPType(*baseType);
      baseType->m_iPType = range_start;
      addType(baseType, true);
    }
  }
  file.close();
  return true;
}

const IPType* IPTypes::getIPType(int protocol, int port) const
{
	std::unordered_map<int, std::unordered_map<int, IPType *>* >::const_iterator it_top = m_ipTypes.find(protocol);
	if (it_top == m_ipTypes.end())
		return nullptr;

	std::unordered_map<int, IPType *>::const_iterator it_secondary = it_top->second->find(port);
	if (it_secondary == it_top->second->end()) {
		if (port == -1)
			return nullptr;
		it_secondary = it_top->second->find(-1);
		if (it_secondary == it_top->second->end())
			return nullptr;
	}
	return it_secondary->second;
}

bool IPTypes::hasType(int protocol, int port) const {
  std::unordered_map<int, std::unordered_map<int, IPType *>* >::const_iterator it_top = m_ipTypes.find(protocol);
  if (it_top == m_ipTypes.end())
    return false;

  std::unordered_map<int, IPType *>::const_iterator it_secondary = it_top->second->find(port);
  if (it_secondary == it_top->second->end()) {
    return false;
  }
  return true;
}

std::ostream& IPTypes::print(std::ostream& x) const
{
  x << "  #  V 6 IP MAC Port Description" << std::endl;
  std::vector<int> type_keys;

  type_keys.reserve (m_ipTypes.size());
  for (auto& it : m_ipTypes) {
      type_keys.push_back(it.first);
  }
  std::sort (type_keys.begin(), type_keys.end());

  for (auto& it_types : type_keys) {

	const std::unordered_map<int, IPType *>* list_of_types = m_ipTypes.at(it_types);

	std::vector<int> ports;
	ports.reserve (list_of_types->size());
	for (auto& it : *list_of_types) {
	  ports.push_back(it.first);
	}
	std::sort (ports.begin(), ports.end());
	for (auto& port : ports) {
		x << *list_of_types->at(port) << std::endl;
	}
  }

	/**
	for (auto it : m_ipTypes) {
		for (auto it2 : *(it.second))
			x << *it2.second << std::endl;
	}
	**/
	return x;
}
