
#define CAN_USE_FILESYSTEM 1

#ifdef CAN_USE_FILESYSTEM
#include <filesystem> // C++ 17
#endif

#include <iostream>
#include <iomanip>
#include <pwd.h>        // getpwuid
#include <sys/stat.h>
#include <unistd.h>     // getuid

#include "utilities.h"

std::string replaceAll(const std::string& s, const std::string& from, const std::string& to) {
  if(from.empty() || s.empty())
    return s;
  std::string str(s);
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

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

bool reduce_input_string(std::string& line) {
  // Convert all white space to spaces.
  convert_all_spaces(line);
  //
  // Compress multiple spaces into a single space.
  //
  std::string::iterator new_end = std::unique(line.begin(), line.end(), BothAreSpaces);
  line.erase(new_end, line.end()); 
  //
  // remove leading and trailing spaces from the string.
  //
  trim(line);
  //
  // Ignore blank lines and lines begining with the '#' character. 
  //
  if (line.front() == '#') {
    line.clear();
  }
  return line.empty();
}

bool parse_reduce_line(std::string& line, std::queue<std::string>& lineq, int num_int_cols) {
  // std::queue does not have an easy way to clear it.
  // Assigning to an empty list is faster than using std::swap against an empty queue.
  lineq = {};
  if (reduce_input_string(line)) {
    return true;
  }
  if (num_int_cols < 0) {
    return true;
  }
  // Now parse the columns.
  const std::string one_space = " ";
  size_t pos = line.find(one_space);
  size_t initialPos = 0;
  int num_found = 0;

  while( pos != std::string::npos ) {
      lineq.push( line.substr( initialPos, pos - initialPos ) );
      initialPos = pos + 1;
      ++num_found;
      if (num_found == num_int_cols) {
        // The rest is the description
        pos = std::string::npos;
      } else {
        pos = line.find( one_space, initialPos );
      }
  }
  lineq.push( line.substr( initialPos, std::min( pos, line.size() ) - initialPos + 1 ) );
  ++num_found;

  if (num_found < num_int_cols) {
    std::cout << "INVALID LINE: " << line << std::endl;
    return true;
  }
  return false;
}

void parse_range(const std::string& token, int& range_start, int range_end, int base) {
  std::string token_start = token;
  std::string token_end;
  size_t pos;
  const std::string minus_sign = "-";
  if ((pos = token_start.find(minus_sign)) != std::string::npos) {
    token_end = token_start;
    token_start = token_end.substr(0, pos);
    token_end.erase(0, pos + minus_sign.length());
  }

  range_start = std::stoi(token_start, nullptr, base);
  range_end = range_start;
  //std::cout << token_start << " = " << std::stoi(token_start, nullptr, base);
  if (token_end.length() > 0) {
    //std::cout << " - " << token_end;
    range_end = std::stoi(token_end, nullptr, base);
  }

  if (range_end < range_start) {
    std::swap(range_end, range_start);
  }
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

#ifdef CAN_USE_FILESYSTEM
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
#endif

bool isPathExist(const std::string& sPath, bool isFile, bool isDirectory, bool canRead, bool canWrite ) {
  bool rc = true;

#ifdef CAN_USE_FILESYSTEM
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
    } 
    if (rc && isFile) {
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
    if (rc && canWrite) {
      rc = (perms & std::filesystem::perms::owner_write)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::group_write)  != std::filesystem::perms::none ||
           (perms & std::filesystem::perms::others_write) != std::filesystem::perms::none ;
    }
  }
#else
  // C++11
  //std::ifstream file(sPath);
  //return file.is_open(sPath);

  // Old School!
  
  struct stat buf;
  rc = stat(sPath.c_str(), &buf) == 0;
  if (rc) {
    if (isDirectory) {
      rc = buf.st_mode & S_IFDIR;
    } 
    if (rc && isFile) {
      rc = buf.st_mode & S_IFREG;
    }
  }
  if (rc && (canRead || canWrite)) {
    if (canRead) {
      rc = (buf.st_mode & (S_IRUSR | S_IRGRP | S_IROTH))  != 0;
    }
    if (rc && canWrite) {
      rc = (buf.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH))  != 0;
    }
  }
#endif
  return rc;
}

std::string getUserHomeDir() {

  const char *homedir;

  // On Windows, I will get something like: 
  // getenv("USEPROFILE") => C:\Users\myuser
  // getenv("HOMEDRIVE")  => C:
  // getenv("HOMEPATH")   => \Users\Myuser

  if ((homedir = getenv("HOME")) == nullptr) {
      homedir = getpwuid(getuid())->pw_dir;
  }
  // Maybe this is Windows!
  if (homedir == nullptr) 
    homedir = getenv("USEPROFILE");
  
  return homedir != nullptr ? homedir : "";
}

std::string getFilename(const std::string& sPath) {
#ifdef CAN_USE_FILESYSTEM
  std::filesystem::path path(sPath);
  return path.filename();
#else
  size_t pos = sPath.find_last_of("\\/");
  if (std::string::npos != pos) ++pos;
  return (std::string::npos == pos) ? sPath : sPath.substr(pos);
#endif
}

std::string getDirectoryFromFilename(const std::string& sPath) {
#ifdef CAN_USE_FILESYSTEM
  std::filesystem::path path(sPath);
  path.remove_filename();
  std::string s = path.string();
  if (s.length() == 0) {
    s = "./";
  }
  return path.string();
#else
  size_t pos = sPath.find_last_of("\\/");
  return (std::string::npos == pos) ? "" : sPath.substr(0, pos);
#endif
}

std::string getFileExtension(const std::string& sPath) {
#ifdef CAN_USE_FILESYSTEM
  std::filesystem::path path(sPath);
  return path.extension();
#else
  size_t pos = sPath.find_last_of(".");
  if (std::string::npos == pos || hasEnding(sPath, ".hidden", true) || 
    sPath.compare(".") == 0 || sPath.compare("..") == 0 || 
    hasEnding(sPath, "/.", true) || hasEnding(sPath, "/..", true)) {
    return "";
  }
  std::string s = sPath.substr(pos);
  return s.find_last_of("\\/") == std::string::npos ? s : "";
#endif
}

bool is_bin_less(const uint8_t *left, const uint8_t *right, size_t len) {
  if (left == right || len == 0) {
    return false;
  }
  if (left == nullptr) {
    // They are not both null because they are not equal.
    return true;
  } else if (right == nullptr) {
    return false;
  }
  
  do {
    --len;
    if (left[len] < right[len]) {
      return true;
    } else if (left[len] > right[len]) {
      return false;
    }
  } while (len > 0);
  return false;
}

bool is_bin_equal(const uint8_t *left, const uint8_t *right, size_t len) {
  if (left == right || len == 0) {
    return true;
  }
  if (left == nullptr || right == nullptr) {
    return false;
  }
  do {
    --len;
    if (left[len] != right[len]) {
      return false;
    }
  } while (len > 0);
  return true;
}

bool is_bin4_less(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return false;
  }
  if (left == nullptr) {
    // They are not both null because they are not equal.
    return true;
  } else if (right == nullptr) {
    return false;
  }
  const uint32_t* left_1 = (const uint32_t*)left;
  const uint32_t* right_1 = (const uint32_t*)right;

  return (*left_1 < *right_1);
}

bool is_bin4_equal(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return true;
  }
  if (left == nullptr || right == nullptr) {
    return false;
  }
  const uint32_t* left_1 = (const uint32_t*)left;
  const uint32_t* right_1 = (const uint32_t*)right;

  return (*left_1 == *right_1);
}


bool is_bin6_less(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return false;
  }
  if (left == nullptr) {
    // They are not both null because they are not equal.
    return true;
  } else if (right == nullptr) {
    return false;
  }
  const uint32_t* left_1 = (const uint32_t*)left;
  const uint32_t* right_1 = (const uint32_t*)right;
  const uint16_t* left_2 = (const uint16_t*)(left + 4);
  const uint16_t* right_2 = (const uint16_t*)(right + 4);

  return (*left_1 < *right_1) || (*left_1 == *right_1 && *left_2 < *right_2);
}

bool is_bin6_equal(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return true;
  }
  if (left == nullptr || right == nullptr) {
    return false;
  }
  const uint32_t* left_1 = (const uint32_t*)left;
  const uint32_t* right_1 = (const uint32_t*)right;
  const uint16_t* left_2 = (const uint16_t*)(left + 4);
  const uint16_t* right_2 = (const uint16_t*)(right + 4);

  return (*left_1 == *right_1) && (*left_2 == *right_2);
}

bool is_bin16_less(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return false;
  }
  if (left == nullptr) {
    // They are not both null because they are not equal.
    return true;
  } else if (right == nullptr) {
    return false;
  }
  const uint64_t* left_1 = (const uint64_t*)left;
  const uint64_t* right_1 = (const uint64_t*)right;
  const uint64_t* left_2 = (const uint64_t*)(left + 8);
  const uint64_t* right_2 = (const uint64_t*)(right + 8);

  return (*left_1 < *right_1) || (*left_1 == *right_1 && *left_2 < *right_2);
}

bool is_bin16_equal(const uint8_t *left, const uint8_t *right) {
  if (left == right) {
    return true;
  }
  if (left == nullptr || right == nullptr) {
    return false;
  }
  const uint64_t* left_1 = (const uint64_t*)left;
  const uint64_t* right_1 = (const uint64_t*)right;
  const uint64_t* left_2 = (const uint64_t*)(left + 8);
  const uint64_t* right_2 = (const uint64_t*)(right + 8);
  //std::cout << "(" << *left_1 << ", " << *right_1 << ")(" << *left_2 << ", " << *right_2 << ")" << std::endl;

  return (*left_1 == *right_1) && (*left_2 == *right_2);
}

std::string getHeuristicFileName(const std::string& pcap_filename, FileTypeEnum fileType, const std::string& output_directory, const std::string& extra_heuristic_name) {
  std::string return_filename = output_directory;
  std::string pcap_extension = getFileExtension(pcap_filename);
  std::string base_filename_with_extension = getFilename(pcap_filename);
  std::string base_filename_no_extension = base_filename_with_extension.substr(0, base_filename_with_extension.size() - pcap_extension.size() + 1);
  std::string forward_slash = "/";

  if (!return_filename.empty() && !hasEnding(return_filename, forward_slash, false)) {
    return_filename.append(forward_slash);
  }

  switch(fileType) {
  case IP_Type      :
    return_filename.append(base_filename_no_extension).append("ip.txt");
    break;
  case MAC_Type     :
    return_filename.append(base_filename_no_extension).append("mac.txt");
    break;
  case CSV_Type     :
    return_filename.append(base_filename_with_extension).append(".csv");
    break;
  case Anomaly_Type :
  case Heuristic_Type :
    return_filename.append(base_filename_no_extension).append(extra_heuristic_name).append(pcap_extension);
    break;
  }
  return return_filename;
}

std::unique_ptr<std::vector<int>> getConstWordLengthVector(const int wordLength, const int n) {
  std::unique_ptr<std::vector<int>> v(new std::vector<int>());
  v->reserve(n);
  for (int i=0; i< n; ++i) {
    v->push_back(wordLength);
  }
  return v;
}

