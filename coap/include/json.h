/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * This is a simple library to convert from C++ to JSON and back again.
 *
 * It down not support lists of different element types.
 *
 * Todos:
 *  - support for null
 *  - test double value with exponent
 */

#pragma once

#ifndef __JSON_h
#define __JSON_h

#include <algorithm>
#include <iterator>
#include <list>
#include <stdexcept>
#include <sstream>
#include <string>

namespace CoAP {

namespace __internal__ {
/**
 * Returns a string without left and right delimiters.
 * Throws runtime_error if the string doesn't have the required delimiters.
 */
inline std::string without(char left, char right, const std::string &s) {
  if (s.front() != left) throw std::runtime_error(s + " did not start with " + left);
  if (s.back() != right) throw std::runtime_error(s + " did not start with " + right);
  return s.substr(1, s.length() - 2);
}

/**
 * Same as without() but specialized for curly brackets.
 */
inline std::string withoutCurly(const std::string &s) {
  return without('{', '}', s);
}

/**
 * Same as without() but specialized for rectangular brackets.
 */
inline std::string withoutRect(const std::string &s) {
  return without('[', ']', s);
}

/**
 * Same as without() but specialized for quotation marks.
 */
inline std::string withoutQuot(const std::string &s) {
  return without('\"', '\"', s);
}

/**
 * Returns the position of the first list delimiter.
 * Returns std::string::npos if there is no list delimiter.
 */
inline size_t findListDelimiter(const std::string &j) {
  auto nesting = 0;
  auto index = 0;
  for (auto c : j) {
    switch (c) {
      case '{':
      case '[':
        ++nesting;
        break;
      case ']':
      case '}':
        --nesting;
        break;
      case ',':
        if (nesting == 0) return index;
        break;
    }
    ++index;
  }
  return std::string::npos;
}

/**
 * Returns the string without leading and trailing spaces.
 */
inline std::string trimmed(const std::string &s) {
  auto start = s.find_first_not_of(' ');
  if (start == std::string::npos) return "";
  auto end = s.find_last_not_of(' ');
  auto count = end != std::string::npos ? (end - start + 1) : end;
  return s.substr(start, count);
}

}  // namespace __internal__

// Object trampoline

/**
 * Returns the JSON representation of the objects.
 */
template <typename T>
std::string to_json(const T& object) {
  return "{" + object.to_json() + "}";
}

/**
 * Initializes the object from the JSON string.
 */
template <typename T>
void from_json(const std::string& json, T& object) {
  object.from_json(__internal__::trimmed(__internal__::withoutCurly(__internal__::trimmed(json))));
}

// Boolean

/**
 * Returns the JSON representation of the boolean value
 */
inline std::string to_json(bool value) {
  return value ? "true" : "false";
}

inline void from_json(const std::string& json, bool& value) {
  value = json == "true";
}

// Numbers

/**
 * Returns the JSON representation of the value.
 */
inline std::string to_json(int value) {
  return std::to_string(value);
}

/**
 * Initializes the value from the JSON string.
 */
inline void from_json(const std::string& json, int& value) {
  value = std::stoi(__internal__::trimmed(json));
}

/**
 * Returns the JSON representation of the value.
 */
inline std::string to_json(unsigned int value) {
  return std::to_string(value);
}

/**
 * Initializes the value from the JSON string.
 */
inline void from_json(const std::string& json, unsigned int& value) {
  value = std::stoul(__internal__::trimmed(json));
}

/**
 * Returns the JSON representation of the value.
 */
inline std::string to_json(double value) {
  auto r = std::to_string(value);
  auto dotPos = r.find_first_of('.');
  auto lastNonZeroPos = r.find_last_not_of('0');
  if (dotPos != std::string::npos) r = r.substr(0, lastNonZeroPos+1);
  return r;
}

/**
 * Initializes the value from the JSON string.
 */
inline void from_json(const std::string& json, double& value) {
  value = std::stod(__internal__::trimmed(json));
}

// Strings

/**
 * Returns the JSON representation of the string.
 */
inline std::string to_json(const char* str) {
  return std::string("\"") + str + '\"';
}


/**
 * Returns the JSON representation of the string.
 */
inline std::string to_json(std::string str) {
  return '\"' + str + '\"';
}

/**
 * Initializes the string from the JSON string.
 */
inline void from_json(const std::string& json, std::string& str) {
  str = __internal__::withoutQuot(__internal__::trimmed(json));
}

// Lists

/**
 * Returns the JSON representation of the list.
 */
template <typename T>
std::string to_json(const std::list<T>& valueList) {
  std::string s;
  if (!valueList.empty()) {
    std::stringstream ss;
    std::list<std::string> ls;
    std::transform(valueList.begin(),
                   valueList.end(),
                   std::back_insert_iterator<std::list<std::string>>(ls),
                   [](const T& t) { return to_json(t); });

    std::copy(ls.begin(), ls.end(), std::ostream_iterator<std::string>(ss, ","));
    s = ss.str();
    s = s.substr(0, s.size() - 1);
  }
  return "[" + s + ']';
}

/**
 * Initializes the list from the JSON string.
 */
template <typename T>
void from_json(const std::string& json, std::list<T>& valueList) {
  valueList.clear();

  struct Helper {
    void operator() (Helper& next, const std::string& json, std::list<T>& valueList) {
      if (json.empty()) return;
      auto pos = __internal__::findListDelimiter(json);
      auto element = __internal__::trimmed(json.substr(0, pos));
      T result;
      from_json(element, result);
      valueList.push_back(result);
      if (pos != std::string::npos) next(next, json.substr(pos + 1), valueList);
    }
  } helper;

  auto elements = __internal__::trimmed(__internal__::withoutRect(__internal__::trimmed(json)));
  if (elements.size()) helper(helper, elements, valueList);
}

// Objects

/**
 * Returns the JSON representation of the object value with the key.
 */
template <typename T>
std::string to_json(const std::string& key, T value) {
  return to_json(key) + ":" + to_json(value);
}

/**
 * Initializes the object value with the key from the JSON string.
 */
template <typename T>
void from_json(const std::string& json, const std::string& key, T& value) {

  struct Helper {
    void operator() (Helper& next, const std::string& json, const std::string& key, T& value) {
      if (json.empty()) return;
      auto commaPos = __internal__::findListDelimiter(json);
      auto element = json.substr(0, commaPos);
      auto colonPos = element.find_first_of(':');
      if (colonPos != std::string::npos) {
        auto k = __internal__::trimmed(element.substr(0, colonPos));
        if (k == key) {
          auto v = __internal__::trimmed(element.substr(colonPos+1));
          from_json(v, value);
          return;
        }
      }
      if (commaPos != std::string::npos) next(next, json.substr(commaPos + 1), key, value);
    }
  } helper;

  if (json.size()) helper(helper, json, to_json(key), value);
}

}  // namespace CoAP

#endif  // __JSON_h
