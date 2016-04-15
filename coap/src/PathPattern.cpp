/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PathPattern.h"

bool PathPattern::match(const Path& path) {
  auto partsToCompare = path.size();
  if (_pattern.size() > path.size()) {
    return false;
  } else if (_pattern.size() < path.size()) {
    if (_pattern.getPart(_pattern.size() - 1) == "*") {
      partsToCompare = _pattern.size() - 1;
    } else {
      return false;
    }
  }

  for (int i = 0; i < partsToCompare; ++i) {
    if (_pattern.getPart(i) == "?") continue;
    if (_pattern.getPart(i) == "*") return true;
    if (_pattern.getPart(i) != path.getPart(i)) return false;
  }

  return true;
}