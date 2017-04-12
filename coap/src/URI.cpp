/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "URI.h"

#include <stdlib.h>

URI URI::fromString(const std::string& string) {
  URI uri;

  const auto firstColon = string.find_first_of(':');
  const auto secondColon = string.find_first_of(':', firstColon + 1);

  const auto firstSlash = string.find_first_of('/');
  const auto secondSlash = string.find_first_of('/', firstSlash + 1);
  const auto thirdSlash = string.find_first_of('/', secondSlash + 1);

  if (firstColon == std::string::npos ||
      firstSlash == std::string::npos ||
      secondSlash == std::string::npos ||
      thirdSlash == std::string::npos) {
    return URI();
  }

  uri.protocol_ = string.substr(0, firstColon);
  uri.server_ = string.substr(secondSlash + 1, std::min(secondColon, thirdSlash) - secondSlash - 1);
  if (secondColon < thirdSlash) {
    const auto portString = string.substr(secondColon + 1, thirdSlash - secondColon - 1);
    uri.port_ = std::stoul(portString);
  } else {
    if (uri.protocol_ == "coap") uri.port_ = 5683;
    if (uri.protocol_ == "coaps") uri.port_ = 20220;
  }
  uri.path_ = string.substr(thirdSlash);
  uri.isValid_ = true;

  return uri;
}
