/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __URI_h
#define __URI_h

#include <string>

class URI {
 public:
  static URI fromString(const std::string& string);

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
  uint16_t port_{5683};
  std::string path_;
};

#endif //__URI_h
