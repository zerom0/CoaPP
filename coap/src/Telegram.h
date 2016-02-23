/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Telegram_h
#define __Telegram_h

#include <netinet/in.h>
#include <vector>

namespace CoAP {

/**
 * Representation of UDP telegram with origin (ip and port) and payload
 */
class Telegram {
  in_addr_t fromIP_ = 0;
  uint16_t fromPort_ = 0;
  std::vector<uint8_t> message_;

 public:
  Telegram() = default;

  Telegram(in_addr_t fromIP, uint16_t fromPort, std::vector<uint8_t>&& message)
    : fromIP_(fromIP)
    , fromPort_(fromPort)
    , message_(message) { }


  in_addr_t getIP() const {
    return fromIP_;
  }

  uint16_t getPort() const {
    return fromPort_;
  }

  std::vector<uint8_t> getMessage() const {
    return message_;
  }
};

}  // namespace CoAP

#endif //__Telegram_h
