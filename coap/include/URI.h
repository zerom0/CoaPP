/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __URI_h
#define __URI_h

#include <string>
#include "Optional.h"

class URI {
 public:
  /**
   * Parses the given string and creates a URI Object if all required fields are found in the string.
   *
   * Supported formats are:
   *    coap[s]://server[:port]/endpoint
   *
   * If the port is optional and if omitted the default port for coap/coaps will be used.
   *
   * @param uri Textual representation of the URI.
   * @return Either an URI object or nothing if the string is not well formed.
   */
  static Optional<URI> fromString(const std::string& uri);

  const std::string& getProtocol() const {
    return protocol_;
  }

  const std::string& getServer() const {
    return server_;
  }

  uint16_t getPort() const {
    return port_;
  }

  const std::string& getPath() const {
    return path_;
  }

 private:
  std::string protocol_;
  std::string server_;
  uint16_t port_{0};
  std::string path_;
};

#endif //__URI_h
