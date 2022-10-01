#ifndef MAC_ADDRESSES_H
#define MAC_ADDRESSES_H

#include <iostream>
#include <set>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>

#include "utilities.h"


//**************************************************************************
//! Encapsulate a collection of MAC addresses.
/*!
  *
 ***************************************************************************/
class MacAddresses {
public:
    /*! Constructor */
    MacAddresses();

    /*! Destructor, clears all structures, not virtual because assume not subclassed. */
    ~MacAddresses();

    /*! Constructor */
    MacAddresses(const MacAddresses& x);

    /*! Assignment operator */
    const MacAddresses& operator=(const MacAddresses& x);

    //**************************************************************************
    //! Add a MAC if it is NOT already stored here. NULL is ignored. A copy is made.
    /*!
     * \param [in]  mac Six bytes for the mac address.
     * 
     * \returns True if the MAC is added.
     *
     ***************************************************************************/
    bool addMacAddress(const uint8_t *mac);

    //**************************************************************************
    //! Determine if this MAC address is already stored.
    /*!
     * \param [in]  mac Six bytes for the mac address.
     * 
     * \returns True if this MAC is already stored.
     *
     ***************************************************************************/
    bool hasAddress(const uint8_t *mac) const;

    //**************************************************************************
    //! Make a copy of the MAC (using new).
    /*!
     * \param [in]  mac Six bytes for the mac address.
     * 
     * \returns Newly allocated MAC.
     *
     ***************************************************************************/
    uint8_t* dupMacAddress(const uint8_t *mac) const;

    //**************************************************************************
    //! Stream the entire set of MACs sorted. 
    /*!
     * \param [in,out]  Stream.
     * 
     * \returns Reference to the stream
     *
     ***************************************************************************/
    std::ostream& print(std::ostream& x) const;

    //**************************************************************************
    //! Convert a 6 byte MAC to an unsigned 64-bit int.
    /*!
     * \param [in]  mac 
     * 
     * \param [out] out
     *
     ***************************************************************************/
    static void mac_to_int(const uint8_t *mac, u_int64_t *out);

    //**************************************************************************
    //! Convert an unsigned 64-bit int to a 6 byte MAC.
    /*!
     * \param [in]  mac 
     * 
     * \param [out] out
     *
     ***************************************************************************/
    static void int_to_mac(u_int64_t mac, uint8_t *out);

    //**************************************************************************
    //! Print an array of 6 8-bit integers to std::cout (which might be a file) formatted as normal MAC addresses
    /*!
     * \param [in] mac  mac address to print.
     *
     ***************************************************************************/
    static std::string mac_to_str(const uint8_t *mac);

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

    static bool is_address_equal(const uint8_t *left, const uint8_t *right);

    void clear();

    //**************************************************************************
    //! Get a copy of all mac pointers. You do NOT own the contained pointers, do not delete them.
    /*!
     * 
     * \returns A vector containing a the MAC addresses, which are stored as arrays of uint8_t.
     *
     ***************************************************************************/
    std::unique_ptr<std::vector<uint8_t *>> toVector() const;

    // Store the binary addresses. 
    // The custom compare method provides more than a 100x speed improvement
    // for the hasAddress method when searching a few thousand addresses.
    //std::set<uint8_t*, CustomLessthan_bin6> m_unique_macs;

    //MacAddresses::<lambda(const DATA_POINTER&)> hash6 = [](const DATA_POINTER& dp){ uint32_t hash_value = 0; if (dp != nullptr) MurmurHash3_x86_32(dp, 6, &hash_value); return hash_value; };
    //bool equal6 = [](const DATA_POINTER& dp lhs, const DATA_POINTER& rhs){ return is_bin6_equal(lhs, rhs); };
    //std::unordered_set<uint8_t*, CustomHash_bin6, CustomEqual_bin6> m_unique_macs(8, CustomHash_bin6, CustomEqual_bin6);
    std::unordered_set<uint8_t*, CustomHash_bin6, CustomEqual_bin6> m_unique_macs;
};

inline bool MacAddresses::is_address_equal(const uint8_t *left, const uint8_t *right) {
    return is_bin6_equal(left, right);
}

#endif // IPTYPE_H
