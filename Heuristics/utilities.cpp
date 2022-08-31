#include <iostream>
#include <iomanip>

#include "utilities.h"

std::string& convert_all_spaces(std::string& str)
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

bool find_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len)
{
  if (s == nullptr || data == nullptr || num == 0 || len < num)
    return false;

  u_int32_t max_start = len - num;
  bool found_it = false;
  for (u_int32_t iStart = 0; !found_it && iStart <= max_start; ++iStart) {
    found_it = true;
    for (u_int32_t i = 0; found_it && i<num; ++i) {
      found_it = s[i] == data[iStart + i];
    }
  }
  return found_it;
}

bool hasEnding(std::string const &fullString, std::string const &ending, bool isCaseSensitive) {
    if (fullString.length() >= ending.length()) {
      if (isCaseSensitive) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
      } else {
        std::string temp_s(fullString);
        std::string temp_e(ending);
        std::transform(temp_s.begin(), temp_s.end(), temp_s.begin(), tolower);
        std::transform(temp_e.begin(), temp_e.end(), temp_e.begin(), tolower);
        return (0 == temp_s.compare(temp_s.length() - temp_e.length(), temp_e.length(), temp_e));
      }
    } else {
        return false;
    }
}

// goes through data from left to right, but searches for the match in reverse
bool reverse_match(const u_int8_t* s, u_int32_t num, const u_int8_t* data, u_int32_t len)
{
  if (s == nullptr || data == nullptr || num == 0 || len < num)
    return false;

  u_int32_t max_start = len - num;
  bool found_it = false;
  for (u_int32_t iStart = 0; !found_it && iStart <= max_start; ++iStart) {
    found_it = true;
    int i = num;
    while (found_it && i!=0)
    {
      --i;
      found_it = s[i] == data[iStart+1];
    }
  }
  return found_it;
}


void dump_hex(const u_int8_t* data, u_int32_t len)
{
  if (data == nullptr || len == 0) 
    return;

  for (u_int32_t i = 0; i<len; ++i) {
    int x = data[i];
    std::cout << std::hex << x << " ";
  }
  std::cout << std::dec << std::endl;
}
