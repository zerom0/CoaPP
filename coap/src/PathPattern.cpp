/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PathPattern.h"

bool PathPattern::match(const Path& path) {
  auto partsToCompare = path.partCount();
  if (_pattern.partCount() > path.partCount()) {
    return false;
  } else if (_pattern.partCount() < path.partCount()) {
    if (_pattern.part(_pattern.partCount() - 1) == "*") {
      partsToCompare = _pattern.partCount() - 1;
    } else {
      return false;
    }
  }

  for (int i = 0; i < partsToCompare; ++i) {
    if (_pattern.part(i) == "?") continue;
    if (_pattern.part(i) == "*") return true;
    if (_pattern.part(i) != path.part(i)) return false;
  }

  return true;
}