/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "URI.h"

#include <stdlib.h>

Optional<URI> URI::fromString(const std::string& uri) {
  const auto firstColon = uri.find_first_of(':');
  const auto secondColon = uri.find_first_of(':', firstColon + 1);

  const auto firstSlash = uri.find_first_of('/');
  const auto secondSlash = uri.find_first_of('/', firstSlash + 1);
  const auto thirdSlash = uri.find_first_of('/', secondSlash + 1);

  if (firstColon == std::string::npos ||
      firstSlash == std::string::npos ||
      secondSlash == std::string::npos ||
      thirdSlash == std::string::npos) {
    return Optional<URI>();
  }

  if (secondSlash - firstSlash != 1) {
    return Optional<URI>();
  }

  URI uriObject;
  uriObject.protocol_ = uri.substr(0, firstColon);
  uriObject.server_ = uri.substr(secondSlash + 1, std::min(secondColon, thirdSlash) - secondSlash - 1);
  if (secondColon < thirdSlash) {
    const auto portString = uri.substr(secondColon + 1, thirdSlash - secondColon - 1);
    uriObject.port_ = std::stoul(portString);
  } else {
    if (uriObject.protocol_ == "coap") uriObject.port_ = 5683;
    if (uriObject.protocol_ == "coaps") uriObject.port_ = 20220;
  }
  uriObject.path_ = uri.substr(thirdSlash);

  return uriObject;
}
