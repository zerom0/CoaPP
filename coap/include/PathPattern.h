/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __PathPattern_h
#define __PathPattern_h

#include "Path.h"

#include <iosfwd>

/*
 * Class: PathPattern
 *
 * Pattern to match a variety of paths
 * Accepted wildcards are
 *  ? - accepts one level
 *  * - accepts any number of levels (must be last level in the pattern)
 */
 // Examples:
 //   /addressbook/?/phone
 //   /addressbook/smith/*
class PathPattern {
 public:
  /*
   * Constructor
   *
   * Parameters:
   *   pattern - Pattern to be matched
   */
  explicit PathPattern(Path pattern) : _pattern(pattern) { }

  /*
   * Constructor
   *
   * Parameters:
   *   pattern - Pattern to be matched
   */
  explicit PathPattern(std::string pattern) : _pattern(pattern) { }

  /*
   * Method: match
   *
   * Check if the path matches the given pattern
   *
   * Returns:
   *   True if the path matches the pattern
   */
  bool match(const Path& path) const;

 private:
  Path _pattern;
};


#endif  // __PathPattern_h
