
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
	x << m_iPType << " " << m_valid << " " << m_iPv6 << " " << m_dupIP << " " << m_dupMAC << " " << m_description;
	return x;
}

IPTypes::IPTypes()
{
}

IPTypes::~IPTypes()
{
	for (auto it : m_ipTypes) {
		if (it.second != nullptr)
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
	std::unordered_map<int, IPType *>::iterator it = m_ipTypes.find(ipt->m_iPType);
	if (it == m_ipTypes.end())
		m_ipTypes[ipt->m_iPType] = ipt;
	else {
		delete m_ipTypes[ipt->m_iPType];
		m_ipTypes[ipt->m_iPType] = ipt;
	}
}


bool IPTypes::read(const std::string& filename)
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
        if (num_found == 5) {
        	// The rest is the description
        	pos = std::string::npos;
        } else {
        	pos = line.find( delimiter, initialPos );
        }
    }
    lineq.push( line.substr( initialPos, std::min( pos, line.size() ) - initialPos + 1 ) );
    ++num_found;

    if (num_found < 5) {
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

    int range_start = std::stoi(token_start, nullptr, 10);
    int range_end = range_start;
    //std::cout << token_start << " = " << std::stoi(token_start, nullptr, 16);
    if (token_end.length() > 0) {
      //std::cout << " - " << token_end;
      range_end = std::stoi(token_end, nullptr, 10);
    }

    // paranoid!
    if (range_end < range_start) {
      int temp = range_start;
      range_start = range_end;
      range_end = temp;
    }

    IPType* baseType = new IPType();
    baseType->m_iPType = range_start;
    baseType->m_valid = std::stoi(lineq.front(), nullptr) != 0;
    lineq.pop();
    baseType->m_iPv6 = std::stoi(lineq.front(), nullptr) != 0;
    lineq.pop();
    baseType->m_dupIP = std::stoi(lineq.front(), nullptr) != 0;
    lineq.pop();
    baseType->m_dupMAC = std::stoi(lineq.front(), nullptr) != 0;
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


std::ostream& IPTypes::print(std::ostream& x) const
{
	for (auto it : m_ipTypes) {
		if (it.second != nullptr)
			x << *it.second << std::endl;
	}
	return x;
}
