
#include <queue>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include "ethtype.h"
#include "utilities.h"

EthernetType::EthernetType() : 	m_EthernetType(false), m_valid(false), m_iPv6(false), m_dupIP(false), m_dupMAC(false)
{
}

EthernetType::EthernetType(const EthernetType& ipt)
{
	if (this != &ipt)
    {
    	m_EthernetType = ipt.m_EthernetType;
		m_valid = ipt.m_valid;
		m_iPv6 = ipt.m_iPv6;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
}

EthernetType::~EthernetType() {}

const EthernetType& EthernetType::operator=(const EthernetType& ipt)
{
	if (this != &ipt) {
  	m_EthernetType = ipt.m_EthernetType;
		m_valid = ipt.m_valid;
		m_iPv6 = ipt.m_iPv6;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
    return *this;
}

std::ostream& EthernetType::print(std::ostream& x) const
{
	x << std::setw(4) << std::hex << m_EthernetType << " " << m_valid << " " << m_iPv6 << " " << m_dupIP << " " << m_dupMAC << " " << m_description;
	return x;
}

EthernetTypes::EthernetTypes()
{
}

EthernetTypes::~EthernetTypes()
{
	for (auto it : m_EthernetTypes) {
		if (it.second != nullptr)
			delete it.second;
	}
	m_EthernetTypes.clear();
}

void EthernetTypes::addType(const EthernetType& ipt)
{
	addType(new EthernetType(ipt), true);
}

void EthernetTypes::addType(EthernetType* ipt, bool ownit)
{
	if (ipt == nullptr)
		return;
	if (!ownit) {
		addType(*ipt);
		return;
	}
	std::unordered_map<int, EthernetType *>::iterator it = m_EthernetTypes.find(ipt->m_EthernetType);
	if (it == m_EthernetTypes.end())
		m_EthernetTypes[ipt->m_EthernetType] = ipt;
	else {
		delete m_EthernetTypes[ipt->m_EthernetType];
		m_EthernetTypes[ipt->m_EthernetType] = ipt;
	}
}


bool EthernetTypes::read(const std::string& filename, int base)
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

    int range_start = std::stoi(token_start, nullptr, base);
    int range_end = range_start;
    //std::cout << token_start << " = " << std::stoi(token_start, nullptr, 16);
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

    EthernetType* baseType = new EthernetType();
    baseType->m_EthernetType = range_start;
    baseType->m_valid = std::stoi(lineq.front(), nullptr, 10) != 0;
    lineq.pop();
    baseType->m_iPv6 = std::stoi(lineq.front(), nullptr, 10) != 0;
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

    while (range_start < range_end) {
    	++range_start;
    	baseType = new EthernetType(*baseType);
    	baseType->m_EthernetType = range_start;
    	addType(baseType, true);
    }
  }
  return true;
}

bool EthernetTypes::sameExceptType(const EthernetType& lhs, const EthernetType& rhs) const
{
  return lhs.m_dupMAC == rhs.m_dupMAC && lhs.m_dupIP == rhs.m_dupIP && lhs.m_valid == rhs.m_valid && lhs.m_description.compare(rhs.m_description) == 0;
}

std::ostream& EthernetTypes::print(std::ostream& x) const
{
  x << "   Range  V 6 IP MAC Description" << std::endl;
  std::vector<int> keys;

  keys.reserve (m_EthernetTypes.size());
  for (auto& it : m_EthernetTypes) {
      keys.push_back(it.first);
  }
  std::sort (keys.begin(), keys.end());

  const EthernetType* lhs = nullptr;
  int last_type = 0;
  for (auto& it : keys) {

    if (lhs == nullptr) {

      // First time through!
      lhs = m_EthernetTypes.at(it);
      last_type = lhs->m_EthernetType;

    } else {

      const EthernetType* rhs = m_EthernetTypes.at(it);
      if (sameExceptType(*lhs, *rhs) && last_type + 1 == rhs->m_EthernetType) {
        ++last_type;
      } else {

        std::stringstream stream;
        stream << std::hex << lhs->m_EthernetType;
        if (last_type != lhs->m_EthernetType) {
          stream << "-" << std::hex << last_type;
        }
        std::string result( stream.str() );
        transform(result.begin(), result.end(), result.begin(), toupper);
        x << std::setw(9) << result << " " << lhs->m_valid << " " << lhs->m_iPv6 << " " << lhs->m_dupIP << "  " << lhs->m_dupMAC << "   " << lhs->m_description << std::endl;

        lhs = rhs;
        last_type = lhs->m_EthernetType;
      }
    }
  }

  std::stringstream stream;
  stream << std::hex << lhs->m_EthernetType;
  if (last_type != lhs->m_EthernetType) {
    stream << "-" << std::hex << last_type;
  }
  std::string result( stream.str() );
  transform(result.begin(), result.end(), result.begin(), toupper);
  x << std::setw(9) << result << " " << lhs->m_valid << " " << lhs->m_iPv6 << " " << lhs->m_dupIP << "  " << lhs->m_dupMAC << "   " << lhs->m_description << std::endl;

/**
	for (auto it : m_EthernetTypes) {
		if (it.second != nullptr)
			x << *it.second << std::endl;
	}
  **/
	return x;
}
