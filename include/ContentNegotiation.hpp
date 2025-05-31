#pragma once
#include <string>
#include <vector>

/**
 * @brief Splits a given string by commas and trims whitespace.
 *
 * @param s The input string to be split and trimmed.
 *
 * @details
 * This function processes the input string by:
 * 1. Using `std::istringstream` to treat the string as a stream.
 * 2. Calling `std::getline()` to extract tokens separated by commas.
 * 3. Removing leading and trailing spaces, tabs (`\t`), carriage returns (`\r`), and newlines (`\n`).
 * 4. Discarding tokens that consist only of whitespace.
 * 5. Storing the cleaned tokens in a vector.
 *
 * Example:
 * ```
 * std::string input = "  foo , bar  , baz\t";
 * splitAndTrim(input);
 * // Result: ["foo", "bar", "baz"]
 * ```
 */
static std::vector<std::string> splitAndTrim(const std::string s);

bool isAcceptable(const std::string &acceptHeader, const std::string &mimeType);
