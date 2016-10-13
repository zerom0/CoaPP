/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __IConnection_h
#define __IConnection_h

#include "Optional.h"
#include "Telegram.h"

#include <chrono>

namespace CoAP {

class IConnection {
 public:
  virtual ~IConnection() = default;

  /**
   * Sends the telegram.
   *
   * @param telegram  Telegram to send
   *
   * @throws  std::logic_error    when the connection is not open
   * @throws  std::runtime_error  when sending the telegram failed
   */
  virtual void send(Telegram&& telegram) = 0;

  /**
   * Waits for and reads a telegram from the network.
   *
   * @param timeout  Time to wait for a telegram before returning nothing
   *
   * @return Either the telegram or nothing if no telegram was received before the timeout..
   *
   * @throws  std::logic_error  when the connection is not open
   */
  virtual Optional<Telegram> get(std::chrono::milliseconds timeout) = 0;
};

}


#endif //__IConnection_h
