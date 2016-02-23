/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __PathPattern_h
#define __PathPattern_h

#include "Path.h"

#include <iosfwd>

// Pattern to match a variety of paths
// Accepted wildcards are
//  ? - accepts one level
//      Ex. /addressbook/?/phone
//  * - accepts any number of levels (must be last level in the pattern)
// Ex. /addressbook/smith/*
class PathPattern {
 public:
  explicit PathPattern(Path pattern) : _pattern(pattern) { }
  explicit PathPattern(std::string pattern) : _pattern(pattern) { }

  // Check if the path matches the given pattern
  bool match(const Path& path);

 private:
  Path _pattern;
};


#endif  // __PathPattern_h
