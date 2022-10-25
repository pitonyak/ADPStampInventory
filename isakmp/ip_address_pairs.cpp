#include "ip_address_pairs.h"

std::size_t ipv4_pair_hash::operator()(const ipv4_pair &pair) const
{
    // Compute hashes of each address in the pair.
    int32_t first_hash, second_hash;
    MurmurHash3_x86_32(&pair.first.s_addr, sizeof(in_addr::s_addr), &first_hash);
    MurmurHash3_x86_32(&pair.second.s_addr, sizeof(in_addr::s_addr), &second_hash);

    // Combine hashes and ensure order of addresses in the pair does not matter.
    if (pair.first.s_addr > pair.second.s_addr)
    {
        return static_cast<size_t>(first_hash) + 37 * static_cast<size_t>(second_hash);
    }
    return static_cast<size_t>(second_hash) + 37 * static_cast<size_t>(first_hash);
}

/**
 * @brief Overloads the equality operator for the in_addr struct representing IPv4 addresses.
 *
 * @param lhs The in_addr struct on the left-hand side.
 * @param rhs The in_addr struct on the right-hand side.
 * @return True if both in_addr structs are equal; false otherwise.
 */
bool operator==(const struct in_addr &lhs, const struct in_addr &rhs)
{
    return lhs.s_addr == rhs.s_addr;
}

bool ipv4_pair_equal::operator()(const ipv4_pair &lhs, const ipv4_pair &rhs) const
{
    return std::tie(lhs.first, lhs.second) == std::tie(rhs.first, rhs.second) ||
           std::tie(lhs.first, lhs.second) == std::tie(rhs.second, rhs.first);
}

void IPAddressPairSet::add_pair(const struct in_addr &source, const struct in_addr &destination)
{
    ipv4_pairs_.insert({source, destination});
}

bool IPAddressPairSet::has_pair(const struct in_addr &source, const struct in_addr &destination) const
{
    return ipv4_pairs_.find({source, destination}) != ipv4_pairs_.end();
}

size_t IPAddressPairSet::size() const
{
    return ipv4_pairs_.size();
}
