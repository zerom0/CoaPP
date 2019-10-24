/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __IMessaging_h
#define __IMessaging_h

#include "Client.h"
#include "MClient.h"

namespace CoAP {

class RequestHandlers;

/*
 * Class: IMessaging
 *
 * The core messaging system for the CoAP client and server.
 * It can be created with <newMessaging()>.
 */
class IMessaging {
 public:
  virtual ~IMessaging() = default;

  /*
   * Method: loopOnce
   *
   * Executes the message processing loop one time. This is to be used in a
   * simple event loop if no separate thread shall be spawned.
   */
  virtual void loopOnce() = 0;

  /*
   * Method: loopStart
   *
   * Starts a thread that runs the message processing loop until <loopStop()> has been called.
   */
  virtual void loopStart() = 0;

  /*
   * Method: loopStop
   *
   * Stops the message processing loop and terminates the thread created with <loopStart()>.
   */
  virtual void loopStop() = 0;

  /*
   * Method: requestHandler
   *
   * Returns:
   *    The requestHandler for configuration purposes.
   */
  virtual RequestHandlers& requestHandler() = 0;

  /*
   * Method: getClientFor
   *
   * Request a client for sending requests to a specific server.
   *
   * Parameters:
   *    server      - URI of the server (FQDN or IP address)
   *    server_port - UDP port of the server to connect to
   *
   * Returns:
   *    A CoAP <Client> for the communication with the server.
   */
  virtual Client getClientFor(const char* server, uint16_t server_port = 5683) = 0;

  /*
   * Method: getMulticastClient
   *
   * Returns a client for issuing multicast requests to the specified port.
   *
   * Parameters:
   *    server_port - UDP port of the server to connect to
   *
   * Returns:
   *    A CoAP client for the communication with the server.
   */
  virtual MClient getMulticastClient(uint16_t server_port = 5683) = 0;
};

}

#endif //__IMessaging_h
