/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"

#include <stdexcept>

Path::Path(std::string from)
  : path_(from.substr(0, from.find_last_not_of('/') + 1)) {
  std::string::size_type start = 0;
  auto length = path_.length();
  while (start < length) {
    auto next = path_.find('/', ++start);
    if (next == std::string::npos) {
      next = length;
    }
    // TODO: Overflow when count > 255
    const uint8_t count = next - start;
    path_[start - 1] = count;
    start = next;
  }
}


// TODO: Cache the size as it is constant
size_t Path::size() const {
  auto length = path_.length();
  auto count = unsigned{0};
  std::string::size_type pos = 0;

  std::string::size_type old_pos = 0;
  while (pos < length) {
    ++count;
    pos += path_[pos] + 1;
    if (pos <= old_pos) break;
    old_pos = pos;
  }

  return count;
}


std::string Path::getPart(unsigned index) const {
  auto length = path_.length();
  auto count = unsigned{0};
  std::string::size_type pos = 0;

  while (pos < length) {
    if (count == index) {
      // found the n-th part => return it
      return path_.substr(pos + 1, path_[pos]);
    }
    ++count;
    pos += path_[pos] + 1;
  }

  // not found ...
  throw std::range_error("'n' exceeds the number of parts in Path!");
}


Path::Buffer Path::toBuffer() const {
  return Buffer(begin(path_), end(path_));
}


Path Path::fromBuffer(const Buffer& buffer) {
  auto path = Path();
  std::copy(begin(buffer), end(buffer), std::back_inserter(path.path_));
  return path;
}


std::string Path::toString() const {
  auto path = path_;
  auto length = path_.length();
  std::string::size_type pos = 0;

  while (pos < length) {
    path[pos] = '/';
    pos += path_[pos] + 1;
  }

  return path;
}


