/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RestResponse_h
#define __RestResponse_h

#include "Code.h"

#include <netinet/in.h>
#include <string>

namespace CoAP {

/// This class represents the answer from a REST request
class RestResponse {
 public:
  in_addr_t fromIp() const { return fromIP_; }

  RestResponse withSenderIP(in_addr_t fromIP) const {
    auto copy(*this);
    copy.fromIP_ = fromIP;
    return copy;
  }

  uint16_t fromPort() const { return fromPort_; }

  RestResponse withSenderPort(uint16_t fromPort) const {
    auto copy(*this);
    copy.fromPort_ = fromPort;
    return copy;
  }

  Code code() const { return code_; }

  RestResponse withCode(Code code) const {
    auto copy(*this);
    copy.code_ = code;
    return copy;
  }

  std::string payload() const { return payload_; }

  // Learning: In this simple example call by value is faster than call by const ref
  RestResponse withPayload(std::string payload) const {
    auto copy(*this);
    copy.payload_ = payload;
    return copy;
  }

  RestResponse withContentFormat(uint8_t contentFormat) const {
    auto copy(*this);
    copy.hasContentFormat_ = true;
    copy.contentFormat_ = contentFormat;
    return copy;
  }

  bool hasContentFormat() const {
    return hasContentFormat_;
  }

  uint8_t contentFormat() const {
    return contentFormat_;
  }
 private:
  Code code_ = Code::NotFound;
  std::string payload_;
  bool hasContentFormat_{false};
  uint8_t contentFormat_;
  in_addr_t fromIP_;
  uint16_t fromPort_;
};

inline std::ostream& operator<<(std::ostream& os, const RestResponse& r) {
  os << r.code();
  return os;
}

}  // namespace CoAP

#endif  // __RestResponse_h
