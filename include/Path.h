/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Path_h
#define __Path_h

#include <string>
#include <vector>

/**
 * Representation of a path, either filesystem or from a URI
 */
class Path {
 public:
  using Buffer = std::vector<uint8_t>;

  /**
   * Initializes a Path from a string
   */
  explicit Path(std::string from);

  /**
   * Returns the number of parts the Path consists of
   */
  unsigned partCount() const;

  /**
   * Returns the n-th part of the Path
   * Throws std::range_error if n is greater than the number of parts
   */
  std::string part(unsigned n) const;

  /**
   * Return the path as buffer
   */
  Buffer asBuffer() const;

  /**
   * Returns string representation of the Path omitting trailing slashes
   */
  std::string toString() const;

  static Path fromBuffer(const Buffer& buffer);

 private:
  Path() = default;

  std::string path_;
};

#endif  // __Path_h

