/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Arguments.h"

#include <algorithm>

namespace {
std::string toLower(const std::string& from) {
  auto copy = from;
  std::transform(copy.begin(), copy.end(), copy.begin(), tolower);
  return copy;
}
}

Optional<Arguments> Arguments::fromArgv(int argc, const char** argv) {
  Arguments arguments;

  if (argc < 3) return Optional<Arguments>();

  auto arg = 1;

  arguments.setRequest(toLower(argv[arg++]));
  auto s = toLower(argv[arg++]);
  if (s.compare("-n") == 0) {
    arguments.setNonConfirmable();
    s = std::string(argv[arg++]);
  }

  auto uri = URI::fromString(s);
  if (!uri) return Optional<Arguments>();
  arguments.setUri(std::move(uri.value()));

  if (arg < argc) {
    arguments.setPayload(argv[arg++]);
  }

  return Optional<Arguments>(arguments);
}
