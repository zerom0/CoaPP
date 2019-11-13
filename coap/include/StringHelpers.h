/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __StringHelpers_h
#define __StringHelpers_h

#include <string>
#include <vector>

/**
 * Splits a string at the first occurrence of the given token.
 *
 * @param string  The string to be split.
 * @param token   The token where the string will be split.
 * @return        A pair with the left and the right part of the token.
 */
inline std::pair<std::string, std::string> splitFirst(const std::string& string, char token) {
  auto pos = string.find_first_of(token);
  return std::make_pair(string.substr(0, pos),
                        (pos != std::string::npos) ? string.substr(pos+1)
                                                   : std::string());
}

/**
 * Splits a string at all occurrences of the given token.
 *
 * @param string  The string to be split.
 * @param token   The token where the string will be split.
 * @return        A vector with all split up parts of the string.
 */
inline std::vector<std::string> splitAll(std::string string, char token) {
  std::vector<std::string> result;

  auto parts = splitFirst(string, token);

  /*
   * ""     -> "",    ""    -> []
   * "abc"  -> "abc", ""    -> ["abc"]
   * ",abc" -> "",    "abc" -> ["", "abc"]
   * "abc," -> "abc", ""    -> ["abc", ""]
   * "a,bc" -> "a",   "bc"  -> ["a", "bc"]
   * ","    -> "",    ""    -> ["", ""]
   */
  while (   !parts.first.empty()
         || string.size()) {
    result.emplace_back(std::move(parts.first));

    if (parts.second.empty() && string.back() == token) {
      result.emplace_back("");
      break;
    }

    string = std::move(parts.second);
    parts = splitFirst(string, token);
  }

  return result;
}

#endif  // __StringHelpers_h
