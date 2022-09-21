#include <filesystem> // C++ 17
#include <iostream>
#include <iomanip>
#include <sys/stat.h>

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

void dump_hex(const uint8_t* data, uint32_t len)
{
  if (data == nullptr || len == 0) 
    return;

  for (uint32_t i = 0; i<len; ++i) {
    int x = data[i];
    std::cout << std::hex << x << " ";
  }
  std::cout << std::dec << std::endl;
}


// goes through data from left to right, but searches for the match in reverse
bool reverse_match(const uint8_t* s, uint32_t num, const uint8_t* data, uint32_t len)
{
  if (s == nullptr || data == nullptr || num == 0 || len < num)
    return false;

  uint32_t max_start = len - num;
  bool found_it = false;
  for (uint32_t iStart = 0; !found_it && iStart <= max_start; ++iStart) {
    found_it = true;
    int i = num;
    while (found_it && i!=0)
    {
      --i;
      found_it = s[i] == data[iStart+i];
    }
  }
  return found_it;
}

// goes through data from left ro right and searches forwards.
bool find_match(const uint8_t* s, uint32_t num, const uint8_t* data, uint32_t len)
{
  if (s == nullptr || data == nullptr || num == 0 || len < num)
    return false;

  uint32_t max_start = len - num;
  bool found_it = false;
  for (uint32_t iStart = 0; !found_it && iStart <= max_start; ++iStart) {
    found_it = true;
    for (uint32_t i = 0; found_it && i<num; ++i) {
      found_it = s[i] == data[iStart + i];
    }
  }
  return found_it;
}

void demo_perms(std::filesystem::perms p)
{
    std::cout << ((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-")
              << ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-")
              << ((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-")
              << ((p & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-")
              << ((p & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-")
              << ((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-")
              << ((p & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-")
              << ((p & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-")
              << ((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-")
              << std::endl;
}

bool isPathExist(const std::string& sPath, bool isFile, bool isDirectory, bool canRead, bool canWrite ) {
  bool rc = true;

  // C++17
  std::filesystem::path path(sPath);
  // Need the error code to avoid throwing an exception.
  std::error_code ec;
  rc = std::filesystem::exists(sPath);
  if (rc) {
    //std::cout << " it exists" << std::endl;
    if (isDirectory) {
      rc = std::filesystem::is_directory(path, ec);
      //std::cout << "is dir: " << rc << std::endl;
    } else if (isFile) {
      rc = std::filesystem::is_regular_file(path, ec);
      //std::cout << "is file: " << rc << std::endl;
    }
  }
  if (rc && (canRead || canWrite)) {
    auto perms = std::filesystem::status(path, ec).permissions();
    //demo_perms(perms);
    if (canRead) {
      rc = (perms & std::filesystem::perms::owner_read)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::group_read)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::others_read) != std::filesystem::perms::none;
    }
    if (canWrite) {
      rc = (perms & std::filesystem::perms::owner_write)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::group_write)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::others_write) != std::filesystem::perms::none ;
    }
  }

  // C++11
  //std::ifstream file(sPath);
  //return file.is_open(sPath);

  // Old School!
  /**
  struct stat buf;
  if (stat(sPath.c_str(), &buf) != 0) {
    rc = false;
  } else if (isDirectory) {
      rc = s.st_mode & S_IFDIR;
    } else if (isFile) {
      rc = s.st_mode & S_IFREG;
    }
  }
  **/
  return rc;
}

std::string getDirectoryFromFilename(const std::string& sPath) {
  std::filesystem::path path(sPath);
  path.remove_filename();
  std::string s = path.string();
  if (s.length() == 0) {
    s = "./";
  }
  return path.string();
}
