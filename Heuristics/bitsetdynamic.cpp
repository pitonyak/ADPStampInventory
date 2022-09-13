
#include "bitsetdynamic.h"

#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>


BitsetDynamic::BitsetDynamic() : m_numBits(0), m_arraySize(0), m_bits(nullptr) {
}

BitsetDynamic::BitsetDynamic(std::size_t n) : BitsetDynamic() {
	if (n > 0) {
		int bit = n % BITS_PER_UNIT;
		int byte = n / BITS_PER_UNIT;
		if (bit > 0) {
			++byte;
		}
		m_bits = new uint64_t[byte];
		m_arraySize = byte;
		for (std::size_t i = 0; i<m_arraySize; ++i) {
			m_bits[i] = 0;
		}

	}
}

void BitsetDynamic::resetSize(std::size_t n) {
	if (m_bits != nullptr) {
		delete[] m_bits;
	}
	if (n > 0) {
		int bit = n % BITS_PER_UNIT;
		int byte = n / BITS_PER_UNIT;
		if (bit > 0) {
			++byte;
		}
		m_bits = new uint64_t[byte];
		m_arraySize = byte;
		m_numBits = n;
		for (std::size_t i = 0; i<m_arraySize; ++i) {
			m_bits[i] = 0;
		}
	}
}


BitsetDynamic::~BitsetDynamic() {
	if (m_bits != nullptr) {
		delete[] m_bits;
	}
}

BitsetDynamic::BitsetDynamic(const BitsetDynamic& x) {
	m_arraySize = x.m_arraySize;
	m_numBits = x.m_numBits;
	if (m_arraySize > 0) {
		m_bits = new uint64_t[m_arraySize];
		for (std::size_t i = 0; i < m_arraySize; ++i) {
			m_bits[i] = x.m_bits[i];
		}
	}
}

const BitsetDynamic& BitsetDynamic::operator=(const BitsetDynamic& x) {
	if (this != &x) {
		m_arraySize = x.m_arraySize;
		m_numBits = x.m_numBits;
		if (m_arraySize > 0) {
			m_bits = new uint64_t[m_arraySize];
			for (std::size_t i = 0; i < m_arraySize; ++i) {
				m_bits[i] = x.m_bits[i];
			}
		}
	}
	return *this;
}

bool BitsetDynamic::operator==(const BitsetDynamic& rhs) const {
	if (this != &rhs) {
		if (m_numBits == rhs.m_numBits) {
			for (std::size_t i = 0; i<m_arraySize; ++i) {
				if (m_bits[i] != rhs.m_bits[i])
					return false;
			}
		} else {
			std::cerr << "Different number of bits in operator==" << std::endl;
		}

	}
	return true;
}

bool BitsetDynamic::at(std::size_t pos) const {
	if (pos < m_numBits) {
		int bit = pos % BITS_PER_UNIT;
		int byte = pos / BITS_PER_UNIT;
		return ((m_bits[byte] & (UINT64_C(1) << bit)) != 0);
	} else {
		std::cerr << "cannot access bit at(" << pos << ") it is out of range." << std::endl;
	}
	return false;
}


bool BitsetDynamic::all() const {
	return count() == m_numBits;
}

bool BitsetDynamic::any() const {
	for (std::size_t i = 0; i < m_arraySize; ++i) {
		if (m_bits[i] != 0) {
			return true;
		}
	}
	return false;
}

bool BitsetDynamic::none() const {
	for (std::size_t i = 0; i < m_arraySize; ++i) {
		if (m_bits[i] != 0) {
			return false;
		}
	}
	return true;
}

std::size_t BitsetDynamic::count() const {
	// The fastest way to count bits is to NOT look at 
	// each bit, but rather, to store an array of values, say
	// in groups of 8. I will not, however, prematurely optimize
	// this. 
	std::size_t counter = 0;
	uint64_t x;
	for (std::size_t i = 0; i < m_arraySize; ++i) {
		x = m_bits[i];
		while (x != 0) {
			if (x & UINT64_C(1)) {
				++counter;
			}
			x = x>>1;
		}
	}
	return counter;
}


const BitsetDynamic& BitsetDynamic::operator&=(const BitsetDynamic& rhs) {
	if (this != &rhs) {
		if (m_numBits == rhs.m_numBits) {
			for (std::size_t i = 0; i<m_arraySize; ++i) {
				m_bits[i] &= rhs.m_bits[i];
			}
		} else {
			std::cerr << "Different number of bits in operator|=" << std::endl;
		}

	}
	return *this;
}

const BitsetDynamic& BitsetDynamic::operator|=(const BitsetDynamic& rhs) {
	if (this != &rhs) {
		if (m_numBits == rhs.m_numBits) {
			for (std::size_t i = 0; i<m_arraySize; ++i) {
				m_bits[i] |= rhs.m_bits[i];
			}
		} else {
			std::cerr << "Different number of bits in operator&=" << std::endl;
		}

	}
	return *this;
}

void BitsetDynamic::setBit(std::size_t pos, bool flag) {
	if (pos < m_numBits) {
		int bit = pos % BITS_PER_UNIT;
		int byte = pos / BITS_PER_UNIT;
		if (flag) {
			m_bits[byte] |= (UINT64_C(1) << bit);
		} else {
			m_bits[byte] &= ~(UINT64_C(1) << bit);
		}
	} else {
		std::cerr << "cannot set bit " << pos << " it is out of range." << std::endl;
	}
}

void BitsetDynamic::clearAllBits() {
	if (m_numBits > 0) {
		for (std::size_t i = 0; i<m_arraySize; ++i) {
			m_bits[i] = 0;
		}
	}
}

void BitsetDynamic::setAllBits() {
	if (m_numBits > 0) {
		for (std::size_t i = 0; i<m_arraySize; ++i) {
			m_bits[i] = std::numeric_limits<uint64_t>::max();
		}
		int bit = m_numBits % BITS_PER_UNIT;
		if (bit > 0) {
			// Clear the high bits by shifting to the right.
			m_bits[m_arraySize - 1] = m_bits[m_arraySize - 1] >> (BITS_PER_UNIT - bit);
		}
	}
}

std::string BitsetDynamic::toString() const {
	std::stringstream stream;
	std::size_t i = m_numBits;
	while (i > 0) {
		--i;
		int bit = i % BITS_PER_UNIT;
		int byte = i / BITS_PER_UNIT;
		if ((m_bits[byte] & (UINT64_C(1) << bit)) != 0) {
			stream << "1";
		} else {
			stream << "0";
		}
	
	}
	return stream.str();
}

