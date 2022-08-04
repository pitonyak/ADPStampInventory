
#include "utilities.h"

std::string& convert_all_spaces(std::string& str, const std::string& chars)
{
  std::replace(str.begin(), str.end(), '\n', ' ');
  std::replace(str.begin(), str.end(), '\t', ' ');
  std::replace(str.begin(), str.end(), '\v', ' ');
  std::replace(str.begin(), str.end(), '\f', ' ');
  std::replace(str.begin(), str.end(), '\r', ' ');
  return str;
}

//
// If using boost then we have functions to trim strings. 
// We do not want to add boost as a dependency, so just build our own left and right trim functions
//
// trim from start (in place)
std::string& ltrim(std::string& str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

// trim from end (in place)
std::string& rtrim(std::string& str, const std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

// trim from both ends (in place)
std::string& trim(std::string& str, const std::string& chars)
{
    return ltrim(rtrim(str, chars), chars);
}
// trim from start (copying)
std::string ltrim_copy(std::string s) {
  ltrim(s);
  return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s) {
  rtrim(s);
  return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s) {
  trim(s);
  return s;
}

//
// Return true if both the lhs and rhs are the space character.
//
bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

