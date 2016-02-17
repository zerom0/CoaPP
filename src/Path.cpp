/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"

#include <stdexcept>

Path::Path(std::string from)
  : path_(from) {
  // remove trailing slashes
  auto length = path_.length();
  while (path_[length - 1] == '/') --length;
  path_.resize(length);

  std::string::size_type start = 0;
  while (start < length) {
    auto next = path_.find('/', ++start);
    if (next == std::string::npos) {
      next = length;
    }
    auto count = next - start;
    path_[start - 1] = count;
    start = next;
  }
}


unsigned Path::partCount() const {
  auto length = path_.length();
  auto count = unsigned{0};
  std::string::size_type pos = 0;

  while (pos < length) {
    ++count;
    pos += path_[pos] + 1;
  }

  return count;
}


std::string Path::part(unsigned n) const {
  auto length = path_.length();
  auto count = unsigned{0};
  std::string::size_type pos = 0;

  while (pos < length) {
    if (count == n) {
      // found the n-th part => return it
      return path_.substr(pos + 1, path_[pos]);
    }
    ++count;
    pos += path_[pos] + 1;
  }

  // not found ...
  throw std::range_error("'n' exceeds the number of parts in Path!");
}


Path::Buffer Path::asBuffer() const {
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


