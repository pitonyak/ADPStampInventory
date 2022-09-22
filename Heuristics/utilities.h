#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm> 
#include <cstdint>
#include <queue>
#include <string>


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
//! Assumes that sPath references a file.
/*!
 * 
 * \param [in] sPath - Path of interest
 * 
 * \returns Directory path.
 *
 ***************************************************************************///
std::string getDirectoryFromFilename(const std::string& sPath);

#endif // UTILITIES_H
