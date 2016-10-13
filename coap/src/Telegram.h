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
 * Representation of UDP telegram with address (ip and port) and payload
 */
class Telegram {
  in_addr_t ip_ = 0;
  uint16_t port_ = 0;
  std::vector<uint8_t> message_;

 public:
  Telegram() = default;

  Telegram(in_addr_t ip, uint16_t port, std::vector<uint8_t> message)
    : ip_(ip)
    , port_(port)
    , message_(std::forward<std::vector<uint8_t>>(message)) { }


  in_addr_t getIP() const {
    return ip_;
  }

  uint16_t getPort() const {
    return port_;
  }

  std::vector<uint8_t> getMessage() const {
    return message_;
  }
};

}  // namespace CoAP

#endif //__Telegram_h
