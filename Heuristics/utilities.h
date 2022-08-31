#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <algorithm> 


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
 * \param [in] str - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string ltrim_copy(std::string s);

//**************************************************************************
//! Return a copy of the string with white space removed from the back (end) of a string. The original is not modified.
/*!
 * 
 * \param [in] str - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string rtrim_copy(std::string s);

//**************************************************************************
//! Return a copy of the string with white space removed from the front and back of a string. The original is not modified.
/*!
 * 
 * \param [in] str - Original string. A copy is made and the copy is modified. The original is left unchanged.
 * 
 * \param [in] chars - White space characters to trim. Defaults to "\t\n\v\f\r ".
 * 
 * \returns a copy of the modified string.
 *
 ***************************************************************************///
std::string trim_copy(std::string s);

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
//! Determin if num bytes from s are contained in data.
/*!
 * This is used to remove multiple runs of spaces as follows. 
 * @code{.cpp}
 *   std::line = "sample  text";
 *   std::string::iterator new_end = std::unique(line.begin(), line.end(), BothAreSpaces);
 *   line.erase(new_end, line.end()); 
 * @endcode
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
bool find_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len);
bool reverse_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len);

void dump_hex(const u_int8_t* data, u_int32_t len);

#endif // UTILITIES_H