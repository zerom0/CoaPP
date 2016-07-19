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
  virtual ~IConnection() = default;

  /*
   * Method: send
   *
   * Sends the telegram.
   *
   * Parameters:
   *   telegram - Telegram to send
   *
   * Throws:
   *   Exception when the connection is not open
   */
  // TODO: Report problems through the return code.
  virtual void send(Telegram&& telegram) = 0;

  /*
   * Method: get
   *
   * Waits for and reads a telegram from the network.
   *
   * Returns:
   *   Either the telegram or nothing if nothing was received before the timeout.
   *
   * Throws:
   *   Exception when the connection is not open
   */
  // TODO: Provide timeout as parameter
  virtual Optional<Telegram> get() = 0;
};

}


#endif //__IConnection_h
