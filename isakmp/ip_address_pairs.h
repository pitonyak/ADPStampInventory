#pragma once

#include <unordered_set>

#include <netinet/ip.h>

#include "MurmurHash3.h"

using ipv4_pair = std::pair<struct in_addr, struct in_addr>;

/**
 * @brief Hashing functor for IPv4 address pairs.
 *
 * @note The order of addresses in the pair does not matter. Addresses in a different order will hash to the same value.
 */
struct ipv4_pair_hash
{
    /**
     * @brief Computes the hash of a pair of IPv4 addresses.
     *
     * @param[in] pair A pair of IPv4 address.
     * @return The hash value of the IPv4 address pair.
     */
    std::size_t operator()(const ipv4_pair &pair) const;
};

/**
 * @brief Comparsion functor for checking equality of two IPv4 address pairs.
 *
 * @note The order of addresses in the pair does not matter.
 */
struct ipv4_pair_equal
{
    /**
     * @brief Checks if two IPv4 address pairs are equal, regardless of order.
     *
     * @param[in] lhs The IPv4 address pair on the left-hand side of the equality operator.
     * @param[in] rhs The IPv4 address pair on the right-hand side of the equality operators.
     * @return True if the left-hand and right-hand pairs have the same IPv4 addresses; false otherwise.
     */
    bool operator()(const ipv4_pair &lhs, const ipv4_pair &rhs) const;
};

/**
 * @brief Set of IP address pairs where the ordering does not matter.
 *
 */
class IPAddressPairSet
{
public:
    /**
     * @brief Constructs a new IPAddressPairSet object.
     */
    IPAddressPairSet() = default;

    /**
     * @brief Adds source and destination IPv4 address to the set.
     *
     * @note The order of the given addresses does not matter.
     *
     * @param[in] source The IPv4 source address.
     * @param[in] destination The IPv4 destination address.
     */
    void add_pair(const struct in_addr &source, const struct in_addr &destination);

    /**
     * @brief Checks whether a pair of IPv4 addresses exists within the set.
     *
     * @note The order of the given addresses does not matter.
     *
     * @param[in] source The IPv4 source address.
     * @param[in] destination The IPv4 destination address.
     *
     * @return True if the given pair of addresses is in the set; false otherwise.
     */
    bool has_pair(const struct in_addr &source, const struct in_addr &destination) const;

    /**
     * @brief Returns the number of IP address pairs entries in the set.
     *
     * @return The number of address pairs.
     */
    size_t size() const;

private:
    /// Pairs of IPv4 addresses.
    std::unordered_set<ipv4_pair, ipv4_pair_hash, ipv4_pair_equal> ipv4_pairs_;
};
