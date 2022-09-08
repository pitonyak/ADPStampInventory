#ifndef IP_ADDRESSES_H
#define IP_ADDRESSES_H

#include <iostream>
#include <set>
#include <cstdint>
#include <string>
#include <unordered_map>

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
     * \returns True if added.
     *
     ***************************************************************************/
    bool addIpAddress(const uint8_t *ip, bool isIPv4);

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
    bool hasIpAddress(const uint8_t *ip, bool isIPv4) const;

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

    bool is_ip_equal(const uint8_t *left, const uint8_t *right, bool isIPv4) const;

    void clear();

    std::set<uint8_t *> m_unique_ipv4;
    std::set<uint8_t *> m_unique_ipv6;
};

#endif // IPTYPE_H
