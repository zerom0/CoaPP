/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __IMessaging_h
#define __IMessaging_h

#include "Client.h"
#include "MClient.h"

namespace CoAP {

class RequestHandlerDispatcher;

class IMessaging {
 public:
  virtual ~IMessaging() = default;

  /// Executes the message processing loop one time
  virtual void loopOnce() = 0;

  /// Starts a thread that runs the message processing loop until loopStop() has been called
  virtual void loopStart() = 0;

  /// Ends the message processing loop and terminates the thread created with loopStart()
  virtual void loopStop() = 0;

  /// Returns the requestHandler for configuration purposes
  virtual RequestHandlerDispatcher& requestHandler() = 0;

  /// Returns a client for issuing requests to the specified server and port
  virtual Client getClientFor(const char* server, uint16_t server_port = 5683) = 0;

  /// Returns a client for issuing multicast requests to the specified port
  virtual MClient getMulticastClient(uint16_t server_port = 5683) = 0;
};

}

#endif //__IMessaging_h
