
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <netinet/in.h>
#include <vector>

#include "ipaddresses.h"


std::string IpAddresses::ip_to_str(const uint8_t *ip, bool isIPv4) {
    if (ip == nullptr) {
        return std::string();
    }
    char ip_addr_max[INET6_ADDRSTRLEN]={0};
    if (isIPv4) {
        inet_ntop(AF_INET, ip, ip_addr_max, INET_ADDRSTRLEN);
    } else {
        inet_ntop(AF_INET, ip, ip_addr_max, INET6_ADDRSTRLEN);
    }
    return std::string(ip_addr_max);
}

bool IpAddresses::isIPv4Str(const std::string& ip) {
    return (ip.find(':') == std::string::npos);
}

uint8_t* IpAddresses::str_to_ip(const std::string& ip) {
    bool isIPv4 = isIPv4Str(ip);
    return str_to_ip(ip, isIPv4);
}

uint8_t * IpAddresses::str_to_ip(const std::string& ip, bool isIPv4) {
    uint8_t ip_addr_max[INET6_ADDRSTRLEN]={0};
    if (isIPv4) {
        if (inet_pton(AF_INET, ip.c_str(), ip_addr_max) != 1)
            return nullptr;
    } else {
        if (inet_pton(AF_INET6, ip.c_str(), ip_addr_max) != 1)
            return nullptr;
    }
    return dupIpAddress(ip_addr_max, isIPv4);
}

IpAddresses::IpAddresses() {
}

IpAddresses::IpAddresses(const IpAddresses& x) {
    for (auto const &ptr: x.m_unique_ipv4) {
        m_unique_ipv4.insert(dupIpAddress(ptr, true));
    }
    for (auto const &ptr: x.m_unique_ipv6) {
        m_unique_ipv6.insert(dupIpAddress(ptr, false));
    }
}

const IpAddresses& IpAddresses::operator=(const IpAddresses& x){
    if (this != &x) {
        clear();
        for (auto const &ptr: x.m_unique_ipv4) {
            m_unique_ipv4.insert(dupIpAddress(ptr, true));
        }
        for (auto const &ptr: x.m_unique_ipv6) {
            m_unique_ipv6.insert(dupIpAddress(ptr, false));
        }
    }
    return *this;
}

void IpAddresses::clear() {
    std::for_each(m_unique_ipv4.begin(), m_unique_ipv4.end(), [](uint8_t* ptr){
      delete[] ptr;
    });
    m_unique_ipv4.clear();

    std::for_each(m_unique_ipv6.begin(), m_unique_ipv6.end(), [](uint8_t* ptr){
      delete[] ptr;
    });
    m_unique_ipv6.clear();
}

IpAddresses::~IpAddresses() {
    std::for_each(m_unique_ipv4.begin(), m_unique_ipv4.end(), [](uint8_t* ptr){
      delete[] ptr;
    });
    m_unique_ipv4.clear();

    std::for_each(m_unique_ipv6.begin(), m_unique_ipv6.end(), [](uint8_t* ptr){
      delete[] ptr;
    });
    m_unique_ipv6.clear();
}

bool IpAddresses::addIpAddress(uint8_t *ip, bool isIPv4, bool ownIt) {
    if (ip == nullptr || hasAddress(ip, isIPv4)) {
        return false;
    }
    uint8_t *ip_copy = ownIt ? ip : dupIpAddress(ip, isIPv4);

    if (isIPv4) {
        m_unique_ipv4.insert(ip_copy);
    } else {
        m_unique_ipv6.insert(ip_copy);
    }
    return true;
}

bool IpAddresses::hasAddress(const uint8_t *ip, bool isIPv4) const {
    return isIPv4 ? 
        m_unique_ipv4.find((uint8_t*)ip) != m_unique_ipv4.end() : 
        m_unique_ipv6.find((uint8_t*)ip) != m_unique_ipv6.end();
}

uint8_t* IpAddresses::dupIpAddress(const uint8_t *ip, bool isIPv4) {
    if (ip == nullptr) {
        return nullptr;
    }
    int n = isIPv4 ? 4 : 16;
    uint8_t *x = new uint8_t[n];
    for (int i=0; i<n; ++i) {
        x[i] = ip[i];
    }
    return x;
};

std::ostream& IpAddresses::print(std::ostream& x) const {
    long num_ipv4 = 0;
    long num_ipv6 = 0;
    std::vector<std::string> keys;
    keys.reserve (m_unique_ipv4.size());
    for (auto const &x: m_unique_ipv4) {
        keys.push_back(ip_to_str(x, true));
        ++num_ipv4;
    }

    std::sort (keys.begin(), keys.end());
    for (auto const &ip: keys) {
        x << ip << std::endl;
    }

    keys.clear();
    keys.reserve (m_unique_ipv6.size());
    for (auto const &x: m_unique_ipv6) {
        keys.push_back(ip_to_str(x, false));
        ++num_ipv6;
    }

    std::sort (keys.begin(), keys.end());
    for (auto const &ip: keys) {
        x << ip << std::endl;
    }

    std::cout << "wrote " << num_ipv4 << " IPv4 addresses and " << num_ipv6 << " IPv6 sddresses." << std::endl;
    return x;
}

bool IpAddresses::write_file(const std::string& filename) {
	std::ofstream file;
	file.open(filename);
	if (!file) {
		std::cerr << "Error opening file " << filename << " for writing. " << std::strerror(errno) << std::endl;
		return false;
	}
	print(file);
	return true;
}

bool IpAddresses::read_file(const std::string& filename) {
  clear();
  std::ifstream file(filename);
  if(!file.is_open()){
    std::cout << "File not found " << filename << std::endl;
    return false;
  }

  int line_count = 0;
  std::string line;
  std::string delimiter = ":";

  while (std::getline(file, line)) {
    if (line.length() == 0 || line.front() == '#') {
        ++line_count;
        continue;
    }
    bool isIPv4 = isIPv4Str(line);
    uint8_t* ip = str_to_ip(line, isIPv4);
    if (ip != nullptr) {
        if (!addIpAddress(ip, isIPv4, true)) {
            delete[] ip;
        }
    } else {
        std::cerr << "Invalid IP address at line " << line_count << " " << line << std::endl;
    }
    
    ++line_count;
  }
  std::cout << "Read " << line_count << " lines from the IP file." << std::endl;
  return true;
}

std::unique_ptr<std::vector<uint8_t *>> IpAddresses::toVector(bool isIPv4) const {
    std::unique_ptr<std::vector<uint8_t *>> v(new std::vector<uint8_t *>());
    if (isIPv4) {
        v->reserve(m_unique_ipv4.size());
            for (auto const &ptr: m_unique_ipv4) {
            v->push_back(ptr);
        }
    } else {
        v->reserve(m_unique_ipv6.size());
        for (auto const &ptr: m_unique_ipv6) {
            v->push_back(ptr);
        }
    }
    return v;
}
