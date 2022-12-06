#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm> 
#include <cstdint>
#include <memory>
#include <queue>
#include <string>

#include "MurmurHash3.h"

enum FileTypeEnum { IP_Type, MAC_Type, CSV_Type, Heuristic_Type, Anomaly_Type };

//**************************************************************************
//! Replace all occurrences of one string with another.
/*!
 * 
 * \param [in] s - String that is modified, but the parameter is unchanged.
 * 
 * \param [in] from - Find every instance of this string.
 * 
 * \param [in] to - Change to this string.
 * 
 * \returns Copy of s where "from" has been replaced with "to"
 *
 ***************************************************************************///
std::string replaceAll(const std::string& s, const std::string& from, const std::string& to);

//**************************************************************************
//! Convert all contained white space "\t\n\v\f\r" to a single space character.
/*!
 * 
 * \param [in,out] str - String that is modified.
 * 
 * \returns a reference to the modified string.
 *
 ***************************************************************************///
std::string& convert_all_spaces(std::string& str);

//**************************************************************************
//! Trim white space from the front of a string. 
/*!
 * Boost and QT both provide trim functions by default. The C++ standard library
 * does not so we define our own to avoid adding more dependencies.
 * 
 * \param [in,out] str - String that is modified.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a reference to the modified string.
 *
 ***************************************************************************///
std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

//**************************************************************************
//! Trim white space from the back (end) of a string. 
/*!
 * Boost and QT both provide trim functions by default. The C++ standard library
 * does not so we define our own to avoid adding more dependencies.
 * 
 * \param [in,out] str - String that is modified.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a reference to the modified string.
 *
 ***************************************************************************///
std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

//**************************************************************************
//! Trim white space from the front and back of a string. 
/*!
 * Boost and QT both provide trim functions by default. The C++ standard library
 * does not so we define our own to avoid adding more dependencies.
 * 
 * \param [in,out] str - String that is modified.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a reference to the modified string.
 *
 ***************************************************************************///
std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

//**************************************************************************
//! Return a copy of the string with white space removed from the front (start) of a string. The original is not modified.
/*!
 * 
 * \param [in] s - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string ltrim_copy(std::string s);

//**************************************************************************
//! Return a copy of the string with white space removed from the back (end) of a string. The original is not modified.
/*!
 * 
 * \param [in] s - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string rtrim_copy(std::string s);

//**************************************************************************
//! Return a copy of the string with white space removed from the front and back of a string. The original is not modified.
/*!
 * 
 * \param [in] s - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string trim_copy(std::string s);

//**************************************************************************
//! Convert all white space to a space, reduce runs of spaces, trim white space, and remove comments.
/*!
 * 
 * \param [in, out] line - String that is modified.
 * 
 * \returns True if the string is empty after reduction.
 *
 ***************************************************************************///
bool reduce_input_string(std::string& line);

bool parse_reduce_line(std::string& line, std::queue<std::string>& lineq, int num_int_cols);

void parse_range(const std::string& token, int& range_start, int range_end, int base);

//**************************************************************************
//! Determine if two characters are both spaces.
/*!
 * This is used to remove multiple runs of spaces as follows. 
 * @code{.cpp}
 *   std::line = "sample  text";
 *   std::string::iterator new_end = std::unique(line.begin(), line.end(), BothAreSpaces);
 *   line.erase(new_end, line.end()); 
 * @endcode
 * 
 * \param [in] lhs - first character to check (left hand side)
 * 
 * \param [in] rhs - second character to check (right hand side)
 * 
 * \returns true if both characters are spaces.
 *
 ***************************************************************************///
bool BothAreSpaces(char lhs, char rhs);

bool hasEnding(std::string const &fullString, std::string const &ending, bool isCaseSensitive);

//**************************************************************************
//! Determine if num bytes from s are contained in data. Searches s and data left to right.
/*!
 * \param [in] s - Search string to find in data.
 * 
 * \param [in] num - Number of bytes to find (length of s).
 * 
 * \param [in] data - Data to search while looking for s.
 *
 * \param [in] len - Length of data.
 * 
 * \returns true if num bytes from s are found in data.
 *
 ***************************************************************************///
bool find_match(const uint8_t* s, uint32_t num, const uint8_t* data, uint32_t len);

//**************************************************************************
//! Determine if num bytes from s are contained in data. Searches s right to left and data left to right.
/*!
 * 
 * \param [in] s - Search string to find in data.
 * 
 * \param [in] num - Number of bytes to find (length of s).
 * 
 * \param [in] data - Data to search while looking for s.
 *
 * \param [in] len - Length of data.
 * 
 * \returns true if num bytes from s are found in data.
 *
 ***************************************************************************///
bool reverse_match(const uint8_t* s, uint32_t num, const uint8_t* data, uint32_t len);

//**************************************************************************
//! Print a hex dump. 
/*!
 * 
 * \param [in] data - Data to search while looking for s.
 *
 * \param [in] len - Length of data.
 *
 ***************************************************************************///
void dump_hex(const uint8_t* data, uint32_t len);

//**************************************************************************
//! Assumes that sPath references a file or directory. If the path does NOT exist, false is returned.
/*!
 * 
 * \param [in] sPath - Path of interest
 * 
 * \param [in] isFile - If true, the path is checked to see if it references a File
 * 
 * \param [in] isDirectory - If true, the path is checked to see if it references a Directory
 * 
 * \param [in] canRead - If true, the path is checked to see if the user has read access
 * 
 * \param [in] canWrite - If true, the path is checked to see if the user has write access
 * 
 * \returns Directory path.
 *
 ***************************************************************************///
bool isPathExist(const std::string& sPath, bool isFile, bool isDirectory, bool canRead, bool canWrite);

//**************************************************************************
//! This targets / assumes Linux.
/*!
 * 
 * \returns The users home directory.
 *
 ***************************************************************************///
std::string getUserHomeDir();

//**************************************************************************
//! Assumes that sPath references a file.
/*!
 * 
 * \param [in] sPath - Path of interest
 * 
 * \returns Directory path.
 *
 ***************************************************************************///
std::string getDirectoryFromFilename(const std::string& sPath);

//**************************************************************************
//! Assumes that sPath references a file. Mimic std::filesystem::path::extension() in C++17 but works with C++11.
/*!
 * 
 * \param [in] sPath - Path to a file, which is assumed to contain the character "/".
 * 
 * \returns The filename (characters after "/").
 *
 ***************************************************************************///
std::string getFileExtension(const std::string& sPath);

//**************************************************************************
//! Assumes that sPath references a file. Mimic std::filesystem::path::filename() in C++17 but works with C++11.
/*!
 * 
 * \param [in] sPath - Path to a file, which is assumed to contain the character "/".
 * 
 * \returns The filename (characters after "/").
 *
 ***************************************************************************///
std::string getFilename(const std::string& sPath);

//**************************************************************************
//! Generic binary compare for less than. This is half the speed of the specific methods that know the size.
/*!
 * 
 * \param [in] left  - Pointer to one set of data to compare.
 * 
 * \param [in] right - Pointer to one set of data to compare.
 * 
 * \param [in] len   - Number of bytes to compare.
 * 
 * \returns True if the left data compares to less than the right.
 *
 ***************************************************************************///

bool is_bin_less(const uint8_t *left, const uint8_t *right, size_t len);

//**************************************************************************
//! Generic binary compare for equal. This is half the speed of the specific methods that know the size.
/*!
 * 
 * \param [in] left  - Pointer to one set of data to compare.
 * 
 * \param [in] right - Pointer to one set of data to compare.
 * 
 * \param [in] len   - Number of bytes to compare.
 * 
 * \returns True if the left data is the same as the right.
 *
 ***************************************************************************///
bool is_bin_equal(const uint8_t *left, const uint8_t *right, size_t len);

//**************************************************************************
//! Binary compare for less than assuming the data is four bytes (32-bits) long.
/*!
 * 
 * \param [in] left  - Pointer to one set of data to compare.
 * 
 * \param [in] right - Pointer to one set of data to compare.
 * 
 * \param [in] len   - Number of bytes to compare.
 * 
 * \returns True if the left data compares to less than the right.
 *
 ***************************************************************************///
bool is_bin4_less(const uint8_t* left, const uint8_t* right);

//**************************************************************************
//! Binary compare for equal assuming the data is four bytes (32-bits) long.
/*!
 * 
 * \param [in] left  - Pointer to one set of data to compare.
 * 
 * \param [in] right - Pointer to one set of data to compare.
 * 
 * \param [in] len   - Number of bytes to compare.
 * 
 * \returns True if the left data is the same as the right.
 *
 ***************************************************************************///
bool is_bin4_equal(const uint8_t *left, const uint8_t *right);
bool is_bin6_less(const uint8_t *left, const uint8_t *right);
bool is_bin6_equal(const uint8_t *left, const uint8_t *right);
bool is_bin16_less(const uint8_t *left, const uint8_t *right);
bool is_bin16_equal(const uint8_t *left, const uint8_t *right);

typedef uint8_t* DATA_POINTER;

//**************************************************************************
//! Custom compare for 4 bytes (32-bits) for use with std::set, which requires less than. This is used for IPv4.
/*!
 *
\code{.cpp}
std::set<uint8_t*, CustomLessthan_bin4> m_unique_ipv4;
\endcode
 *
 ***************************************************************************///
struct CustomLessthan_bin4 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin4_less(left, right);
    }
};

//**************************************************************************
//! Custom compare for 4 bytes (32-bits) for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin4, CustomEqual_bin4> m_unique_ipv4;
\endcode
 *
 ***************************************************************************///
struct CustomEqual_bin4 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin4_equal(left, right);
    }
};

//**************************************************************************
//! Custom compare for 6 bytes for use with std::set, which requires less than. This is used for MAC addresses.
/*!
 *
\code{.cpp}
std::set<uint8_t*, CustomLessthan_bin6> m_unique_macs;
\endcode
 *
 ***************************************************************************///
struct CustomLessthan_bin6 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin6_less(left, right);
    }
};

//**************************************************************************
//! Custom compare for 6 bytes for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin6, CustomEqual_bin6> m_unique_macs;
\endcode
 *
 ***************************************************************************///
struct CustomEqual_bin6 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin6_equal(left, right);
    }
};

//**************************************************************************
//! Custom compare for 16 bytes for use with std::set, which requires less than. This is used for IPv6.
/*!
 *
\code{.cpp}
std::set<uint8_t*, CustomLessthan_bin16> m_unique_ipv4;
\endcode
 *
 ***************************************************************************///
struct CustomLessthan_bin16 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin16_less(left, right);
    }
};

//**************************************************************************
//! Custom compare for 16 bytes for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin16, CustomEqual_bin16> m_unique_ipv6;
\endcode
 *
 ***************************************************************************///
struct CustomEqual_bin16 {
    bool operator()(const DATA_POINTER& left, const DATA_POINTER& right) const {
        return is_bin16_equal(left, right);
    }
};

//**************************************************************************
//! Wrapper for a hash of 4 bytes (32-bits) for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin4, CustomEqual_bin4> m_unique_ipv4;
\endcode
 *
 ***************************************************************************///
struct CustomHash_bin4
{
    std::size_t operator()(const uint8_t* x) const
    {
        uint32_t hash_value = 0;
        if (x != nullptr) MurmurHash3_x86_32(x, 4, &hash_value);
        return hash_value;
    }
};

//**************************************************************************
//! Wrapper for a hash of 6 bytes for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin6, CustomEqual_bin6> m_unique_macs;
\endcode
 *
 ***************************************************************************///
struct CustomHash_bin6
{
    std::size_t operator()(const uint8_t* x) const
    {
        uint32_t hash_value = 0;
        if (x != nullptr) MurmurHash3_x86_32(x, 6, &hash_value);
        return hash_value;
    }
};

//**************************************************************************
//! Wrapper for a hash of 16 bytes for use with std::unordered_set.
/*!
 *
\code{.cpp}
std::unordered_set<uint8_t*, CustomHash_bin16, CustomEqual_bin16> m_unique_ipv6;
\endcode
 *
 ***************************************************************************///
struct CustomHash_bin16
{
    std::size_t operator()(const uint8_t* x) const
    {
        uint32_t hash_value = 0;
        if (x != nullptr) MurmurHash3_x86_32(x, 16, &hash_value);
        return hash_value;
    }
};

//**************************************************************************
//! Generate filenames from the PCAP based on use. These are the filenames used when generating files.
/*!
 * 
 * Assuming the PCAP filename is name.pcap, the generated filename is:
 * IP_Type  : name.ip.txt
 * MAC_Type : name.mac.txt
 * CSV_Type : name.pcap.csv
 * Anomaly_Type : name.anomaly.pcap
 * 
 * \param [in] pcap_filename Full path to the PCAP file.
 * 
 * \param [in] fileType What type of file?
 * 
 * \returns A filename based on what the file is.
 *
 ***************************************************************************/
std::string getHeuristicFileName(const std::string& pcap_filename, FileTypeEnum fileType, const std::string& output_directory, const std::string& extra_heuristic_name);

//**************************************************************************
//! Generate a vector of length n with constant values. This is used for the AHO Corasick algorithm.
/*!
 * The Aho Corasick algorithm accepts a vector with the things to search and a vector with the
 * length of each thing; because length information is not available from an array of bytes (sadly).
 * 
 * \param [in] wordLength Every value has this value.
 * 
 * \param [in] n Creates a vector of this length.
 * 
 * \returns A vector with the same value in every element.
 *
 ***************************************************************************/
std::unique_ptr<std::vector<int>> getConstWordLengthVector(const int wordLength, const int n);

#endif // UTILITIES_H
