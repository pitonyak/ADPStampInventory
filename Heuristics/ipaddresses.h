#ifndef IP_ADDRESSES_H
#define IP_ADDRESSES_H

#include <iostream>
#include <set>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>

#include "utilities.h"

//**************************************************************************
//! Encapsulate a collection of IP addresses.
/*!
  *
 ***************************************************************************/
class IpAddresses {
public:
    /*! Constructor */
    IpAddresses();

    /*! Constructor */
    IpAddresses(const IpAddresses& x);

    /*! Assignment operator */
    const IpAddresses& operator=(const IpAddresses& x);

    /*! Destructor, clears all structures, not virtual because assume not subclassed. */
    ~IpAddresses();

    //**************************************************************************
    //! Add a IP if it is NOT already stored here. NULL is ignored. A copy is made.
    /*!
     * \param [in]  ip Four or 16 bytes for the IP address.
     * 
     * \param [in] isIPv4 True if IPv4, otherwise IPv6
     * 
     * \param [in] ownIt, if true, you own this pointer and delete it. 
     * 
     * \returns True if added.
     *
     ***************************************************************************/
    bool addIpAddress(uint8_t *ip, bool isIPv4, bool ownIt);

    //**************************************************************************
    //! Determine if this IP address is already stored.
    /*!
     * \param [in] ip Four or 16 bytes for the IP address.
     * 
     * \param [in] isIPv4 True if IPv4, otherwise IPv6
     * 
     * \returns True if exists.
     *
     ***************************************************************************/
    bool hasAddress(const uint8_t *ip, bool isIPv4) const;

    //**************************************************************************
    //! Make a copy of the IP (using new).
    /*!
     * \param [in]  ip Four or 16 bytes for the IP address.
     * 
     * \param [in] isIPv4 True if IPv4, otherwise IPv6
     * 
     * \returns Newly allocated MAC.
     *
     ***************************************************************************/
    static uint8_t* dupIpAddress(const uint8_t *ip, bool isIPv4);

    /*! Simply method to print this object as a line with no context. */
    std::ostream& print(std::ostream& x) const;

    //**************************************************************************
    //! Print an array of 6 8-bit integers to std::cout (which might be a file) formatted as normal MAC addresses
    /*!
     * \param [in]  ip Four or 16 bytes for the IP address.
     * 
     * \param [in] isIPv4 True if IPv4, otherwise IPv6
     * 
     * \returns A string version of the IP.
     *
     ***************************************************************************/
    static std::string ip_to_str(const uint8_t *ip, bool isIPv4);

    static uint8_t * str_to_ip(const std::string& ip);
    static uint8_t * str_to_ip(const std::string& ip, bool isIPv4);
    static bool isIPv4Str(const std::string& ip);

    //**************************************************************************
    //! Check if the ip address is a valid multicast address or not.
    /*!
     * \param [in]  ip address in integer format.
     * 
     * \returns true if ip address is a multicast address, false otherwise.
     *
     ***************************************************************************/
    static bool is_multicast_address(in_addr_t ip);

    //**************************************************************************
    //! Read a file of MAC addresses such as 00:0e:29:25:73:00 with very limited error checking. One line per MAC.
    /*!
     * \param [in] filename  Path to file containing the MAC addresses.
     * 
     * \returns True if no error is noticed; false otherwise. 
     *
     ***************************************************************************/
    bool read_file(const std::string& filename);

    //**************************************************************************
    //! Write a file of MAC addresses such as 00:0e:29:25:73:00. The lines are sorted. 
    /*!
     * \param [in] filename  Path to file containing the MAC addresses.
     * 
     * \returns True if no error is noticed; false otherwise. 
     *
     ***************************************************************************/
    bool write_file(const std::string& filename);

    bool is_address_equal(const uint8_t *left, const uint8_t *right, bool isIPv4) const;

    void clear();

    int sizeIPv4() const { return m_unique_ipv4.size(); }
    int sizeIPv6() const { return m_unique_ipv6.size(); }

    std::unique_ptr<std::vector<uint8_t *>> toVector(bool isIPv4) const;

    // Store the binary addresses. 
    // The custom compare method provides more than a 100x speed improvement
    // for the hasAddress method when searching a few thousand addresses.
    //std::set<uint8_t*, CustomLessthan_bin4> m_unique_ipv4;
    //std::set<uint8_t*, CustomLessthan_bin16> m_unique_ipv6;
    std::unordered_set<uint8_t*, CustomHash_bin4, CustomEqual_bin4> m_unique_ipv4;
    std::unordered_set<uint8_t*, CustomHash_bin16, CustomEqual_bin16> m_unique_ipv6;
};

inline bool IpAddresses::is_address_equal(const uint8_t *left, const uint8_t *right, bool isIPv4) const {
    return isIPv4 ? is_bin4_equal(left, right) : is_bin16_equal(left, right);
}



#endif // IPTYPE_H
