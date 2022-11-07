#pragma once

#include <string>

/**
 * @brief Removes characters from the left side of a string.
 *
 * @param[in,out] str The string to be trimmed of whitespace.
 * @param[in] chars The characters to be removed. By default this is a set of whitespace characters.
 * @return A reference to the trimmed string.
 */
std::string &ltrim(std::string &str, const std::string &chars = "\t\n\v\f\r ");

/**
 * @brief Removes characters from the right side of a string.
 *
 * @param[in,out] str The string to be trimmed of whitespace.
 * @param[in] chars The characters to be removed. By default this is a set of whitespace characters.
 * @return A reference to the trimmed string.
 */
std::string &rtrim(std::string &str, const std::string &chars = "\t\n\v\f\r ");

/**
 * @brief Removes characters from the left and right sides of a string.
 *
 * @param[in,out] str The string to be trimmed of whitespace.
 * @param[in] chars The characters to be removed. By default this is a set of whitespace characters.
 * @return A reference to the trimmed string.
 */
std::string &trim(std::string &str, const std::string &chars = "\t\n\v\f\r ");
