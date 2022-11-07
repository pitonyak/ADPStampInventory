#include "string_util.h"

std::string &ltrim(std::string &str, const std::string &chars)
{
    // Remove chars from the left side of the string in place.
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string &rtrim(std::string &str, const std::string &chars)
{
    // Remove chars from the right side of the string in place.
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string &trim(std::string &str, const std::string &chars)
{
    // Remove chars from both sides of the string in place.
    return ltrim(rtrim(str, chars), chars);
}
