/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __IConnection_h
#define __IConnection_h

#include "Optional.h"
#include "Telegram.h"

namespace CoAP {

class IConnection {
 public:
  using OptionalTelegram = std::pair<bool, Telegram>;

  virtual ~IConnection() = default;

  // Send message to the server address given in the telegram
  // throws exception if the connection is not open
  virtual void send(Telegram&& telegram) = 0;

  // Receive message from the network
  // Returns either with message or after timeout
  // throws exception if the connection is not open
  virtual Optional<Telegram> get() = 0;
};

}


#endif //__IConnection_h
