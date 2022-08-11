
#include <queue>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include "iptype.h"
#include "utilities.h"

IPType::IPType() : 	m_iPType(false), m_valid(false), m_iPv6(false), m_dupIP(false), m_dupMAC(false)
{
}

IPType::IPType(const IPType& ipt)
{
	if (this != &ipt)
    {
    	m_iPType = ipt.m_iPType;
		m_valid = ipt.m_valid;
		m_iPv6 = ipt.m_iPv6;
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
		m_iPv6 = ipt.m_iPv6;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
    return *this;
}

std::ostream& IPType::print(std::ostream& x) const
{
	x << std::setw(4) << m_iPType << " " << m_valid << " " << m_iPv6 << "  " << m_dupIP << "  " << m_dupMAC << " " << std::setw(3) << m_port << "   " << m_description;
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

	std::unordered_map<int, IPType *>* a_type_map = nullptr;
	std::unordered_map<int, std::unordered_map<int, IPType *>* >::iterator it_top = m_ipTypes.find(ipt->m_iPType);
	a_type_map = (it_top == m_ipTypes.end()) ? new std::unordered_map<int, IPType *>() : it_top->second;

	if (it_top == m_ipTypes.end()) {
		m_ipTypes[ipt->m_iPType] = a_type_map;
	}

	std::unordered_map<int, IPType *>::iterator it_secondary = a_type_map->find(ipt->m_port);

	if (it_secondary != a_type_map->end()) {
		delete (*a_type_map)[ipt->m_iPType];
	}

	(*a_type_map)[ipt->m_port] = ipt;
}


bool IPTypes::read(const std::string& filename, int base)
{
	 //
  // Probably not safe to assume that C++17 is availble so do not use <filesystem> such as
  // std::filesystem::path f{"file.txt"};
  // if (std::filesystem::exists(f)) ...
  //
  std::ifstream file(filename);
  if(!file.is_open()){
    std::cout << "File not found " << filename << std::endl;
    return false;
  }

  int line_count = 0;
  std::string line;
  // Note that reading a file using a FILE* is 4 to 5 times faster.
  // Processing speed is not an issue since this is not done often and the file is small.
  while (std::getline(file, line)) {
  	std::string delimiter = " ";
  	++line_count;
    // Make sure that tabs are now spaces, for example. 
    convert_all_spaces(line);
    //
    // Compress multiple spaces into a single space.
    //
    std::string::iterator new_end = std::unique(line.begin(), line.end(), BothAreSpaces);
    line.erase(new_end, line.end()); 
    //
    // remove leading and trailing spaces from the string.
    //
    trim(line);
    //
    // Ignore blank lines and lines begining with the '#' character. 
    //
    if (line.length() == 0 || line.front() == '#') {
      continue;
    }

    // There are five integers followed by a string description.
    // ip type, OK, IPv6, IPs in payload, MACs in payload, Description
    // 
    std::queue<std::string> lineq;

    size_t pos = line.find(delimiter);
    size_t initialPos = 0;
    int num_found = 0;

    while( pos != std::string::npos ) {
        lineq.push( line.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;
        ++num_found;
        if (num_found == 6) {
        	// The rest is the description
        	pos = std::string::npos;
        } else {
        	pos = line.find( delimiter, initialPos );
        }
    }
    lineq.push( line.substr( initialPos, std::min( pos, line.size() ) - initialPos + 1 ) );
    ++num_found;

    if (num_found < 6) {
    	std::cout << "INVALID LINE (" << line_count << ") : " << line << std::endl;
    	continue;
    }

    // Check for a range such as 1-7  
    std::string token_start = lineq.front();
    lineq.pop();
    std::string token_end;
    delimiter = "-";
    if ((pos = token_start.find(delimiter)) != std::string::npos) {
      token_end = token_start;
      token_start = token_end.substr(0, pos);
      token_end.erase(0, pos + delimiter.length());
    }

    int range_start = std::stoi(token_start, nullptr, base);
    int range_end = range_start;
    //std::cout << token_start << " = " << std::stoi(token_start, nullptr, base);
    if (token_end.length() > 0) {
      //std::cout << " - " << token_end;
      range_end = std::stoi(token_end, nullptr, base);
    }

    // paranoid!
    if (range_end < range_start) {
      int temp = range_start;
      range_start = range_end;
      range_end = temp;
    }

    IPType* baseType = new IPType();
    baseType->m_iPType = range_start;
    baseType->m_valid = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_iPv6 = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_dupIP = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_dupMAC = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_port = std::stoi(lineq.front(), nullptr, 10);
    lineq.pop();
    if (!lineq.empty()) {
    	baseType->m_description = lineq.front();
    	lineq.pop();
    }
    addType(baseType, true);

    while (range_start < range_end) {
    	++range_start;
    	baseType = new IPType(*baseType);
    	baseType->m_iPType = range_start;
    	addType(baseType, true);
    }
  }
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
