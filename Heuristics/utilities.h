#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <algorithm> 


std::string& convert_all_spaces(std::string& str, const std::string& chars = "\t\n\v\f\r ");

//
// If using boost then we have functions to trim strings. 
// We do not want to add boost as a dependency, so just build our own left and right trim functions
//
// trim from start (in place)
std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

// trim from end (in place)
std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

// trim from both ends (in place)
std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

// trim from start (copying)
std::string ltrim_copy(std::string s);

// trim from end (copying)
std::string rtrim_copy(std::string s);

// trim from both ends (copying)
std::string trim_copy(std::string s);

//
// Return true if both the lhs and rhs are the space character.
//
bool BothAreSpaces(char lhs, char rhs);

#endif // UTILITIES_H