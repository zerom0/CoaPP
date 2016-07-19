/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Path_h
#define __Path_h

#include <string>
#include <vector>

/*
 * Class: Path
 *
 * Representation of the parts of the URI path separated by forward slashes.
 */
class Path {
 public:
  using Buffer = std::vector<uint8_t>;

  /*
   * Constructor
   *
   * Parameters:
   *   from - String representation of the path.
   */
  explicit Path(std::string from);

  /*
   * Method: size
   *
   * Returns:
   *   The number of parts the path consists of.
   */
  size_t size() const;

  /*
   * Method: getPart
   *
   * Returns:
   *   The n-th part of the path.
   *
   * Throws:
   *   std::range_error if n is greater than the number of parts.
   */
  std::string getPart(unsigned index) const;

  /*
   * Method: toBuffer
   *
   * Encode the path into a buffer suitable to be sent as part of a CoAP message.
   *
   * Returns:
   *   The path as buffer.
   */
  Buffer toBuffer() const;

  /*
   * Method: fromBuffer
   *
   * Decode the path from a buffer that was part of a CoAP message.
   *
   * Parameters:
   *   buffer - Buffer to be decoded
   *
   * Returns:
   *   Decoded path object
   */
  static Path fromBuffer(const Buffer& buffer);

  /*
   * Method: toString
   *
   * Returns:
   *   String representation of the path omitting trailing slashes.
   */
  std::string toString() const;

 private:
  Path() = default;

  std::string path_;
};

#endif  // __Path_h

