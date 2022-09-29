
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>

#include "macaddresses.h"



void MacAddresses::mac_to_int(const uint8_t *mac, u_int64_t *out){
  /*Pack a MAC address (array of 6 unsigned chars) into an integer for easy inclusion in a set*/
  *out = (u_int64_t)(mac[0])<<40;
  *out += (u_int64_t)(mac[1])<<32; 
  *out += (u_int64_t)(mac[2])<<24;
  *out += (u_int64_t)(mac[3])<<16;
  *out += (u_int64_t)(mac[4])<<8;
  *out += (u_int64_t)(mac)[5];
}

void MacAddresses::int_to_mac(u_int64_t mac, uint8_t *out){
  //Populate an array of 6 8-bit integergs from a 48-bit integer that has been packed in a 64-bit integer
  //This is intended to turn an integer into a more human-usable MAC address array
  //   of the kind used by Ethernet structs from PCAP data
  out[0] = mac>>40 & 0xff;
  out[1] = mac>>32 & 0xff;
  out[2] = mac>>24 & 0xff;
  out[3] = mac>>16 & 0xff;
  out[4] = mac>>8 & 0xff;
  out[5] = mac & 0xff;
}

std::string MacAddresses::mac_to_str(const uint8_t *mac) {
      // Print an array of 6 8-bit integers to std::cout (which might be a file) formatted as normal MAC addresses
      std::stringstream ss;
      ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[0]) << ":"
        << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[1]) << ":"
        << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[2]) << ":"
        << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[3]) << ":"
        << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[4]) << ":"
        << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(mac[5]);
      return ss.str();
}


MacAddresses::MacAddresses() {
}

MacAddresses::MacAddresses(const MacAddresses& x) : MacAddresses() {
    for (auto const &mac_ptr: x.m_unique_macs) {
        m_unique_macs.insert(dupMacAddress(mac_ptr));
    }
}

const MacAddresses& MacAddresses::operator=(const MacAddresses& x){
    if (this != &x) {
        clear();
        for (auto const &mac_ptr: x.m_unique_macs) {
            m_unique_macs.insert(dupMacAddress(mac_ptr));
        }
    }
    return *this;
}

void MacAddresses::clear() {
    std::for_each(m_unique_macs.begin(), m_unique_macs.end(), [](uint8_t* mac_ptr){
      delete[] mac_ptr;
    });
    m_unique_macs.clear();
}

MacAddresses::~MacAddresses() {
    std::for_each(m_unique_macs.begin(), m_unique_macs.end(), [](uint8_t* mac_ptr){
      delete[] mac_ptr;
    });
    m_unique_macs.clear();
}

bool MacAddresses::is_address_equal(const uint8_t *left, const uint8_t *right) {
    return is_bin6_equal(left, right);
}

bool MacAddresses::addMacAddress(const uint8_t *mac) {
    if (mac == nullptr || hasAddress(mac)) {
        return false;
    }
    m_unique_macs.insert(dupMacAddress(mac));
    return true;
}

bool MacAddresses::hasAddress(const uint8_t *mac) const {
    return m_unique_macs.find((uint8_t*)mac) != m_unique_macs.end();
}

std::vector<uint8_t *>* MacAddresses::toVector() const {
  std::vector<uint8_t *>* v = new std::vector<uint8_t *>();
  v->reserve(m_unique_macs.size());
  for (auto const &ptr: m_unique_macs) {
    v->push_back(ptr);
  }
  return v;
}

uint8_t* MacAddresses::dupMacAddress(const uint8_t *mac) const {
    if (mac == nullptr) {
        return nullptr;
    }
    uint8_t *x = new uint8_t[6];
    x[0] = mac[0];
    x[1] = mac[1];
    x[2] = mac[2];
    x[3] = mac[3];
    x[4] = mac[4];
    x[5] = mac[5];
    return x;
};

std::ostream& MacAddresses::print(std::ostream& x) const {
	long counter = 0;
    std::vector<std::string> keys;
    keys.reserve (m_unique_macs.size());
    for (auto const &x: m_unique_macs) {
        keys.push_back(mac_to_str(x));
        ++counter;
    }

    std::sort (keys.begin(), keys.end());
    for (auto const &a_mac: keys) {
        x << a_mac << std::endl;
    }

    std::cout << "Wrote " << counter << " MAC addresses." << std::endl;
    return x;
}

bool MacAddresses::write_file(const std::string& filename) {
	std::ofstream file;
	file.open(filename);
	if (!file) {
		std::cerr << "Error opening file " << filename << " for writing. " << std::strerror(errno) << std::endl;
		return false;
	}
	print(file);
	return true;
}

bool MacAddresses::read_file(const std::string& filename) {
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
    if (line.length() != 17) {
        std::cout << "Line " << line_count << " has length " << line.size() << " (expected 17)." << std::endl;
        ++line_count;
        continue;
    }
    size_t pos = line.find(delimiter);
    size_t initialPos = 0;
    int idx = 0;
    std::string s;
    uint8_t mac[6];
    while( pos != std::string::npos ) {
        s = line.substr( initialPos, pos - initialPos );
        mac[idx] = std::stoul(s, nullptr, 16);
        initialPos = pos + 1;
        pos = line.find( delimiter, initialPos );
        ++idx;
    }
    s = line.substr( initialPos, std::min( pos, line.size() ) - initialPos + 1 );
    mac[idx] = std::stoul(s, nullptr, 16);
    addMacAddress(mac);
    ++line_count;
  }
  std::cout << "Read " << line_count << " lines from the MAC file." << std::endl;
  return true;
}
