/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "NetUtils.h"

#include <cstring>
#include <stdexcept>
#include <string>

namespace CoAP {

in_addr_t NetUtils::ipFromHostname(const std::string& hostname) {
  hostent* host = gethostbyname(hostname);

  if (host == 0 ||
      host->h_length == 0) {
    throw std::runtime_error("Server name could not be resolved.");
  }

  in_addr_t addr;
  memcpy(&addr, host->h_addr_list[0], sizeof(addr));

  return addr;
}

hostent* NetUtils::gethostbyname(const std::string& server) const {
  return ::gethostbyname(server.c_str());
}

}  // namespace CoAP
