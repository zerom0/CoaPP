/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Client_h
#define __Client_h

#include "Responses.h"
#include "RestResponse.h"
#include "Observation.h"

#include <string>
#include <future>
#include <netinet/in.h>
#include <map>

namespace CoAP {

class ClientImpl;
class IRequestHandler;

/// The CoAP Client interface provides users with the ability to send REST
/// requests in a simple and synchronous way.
class Client {
 public:
  Client(ClientImpl& impl, const std::string& server, uint16_t server_port);

  // Send GET request to server
  std::future<RestResponse> GET(const std::string& uri, bool confirmable = false);

  // Send PUT request to server
  std::future<RestResponse> PUT(const std::string& uri, const std::string& payload, bool confirmable = false);

  // Send POST request to server
  std::future<RestResponse> POST(const std::string& uri, const std::string& payload, bool confirmable = false);

  // Send DELETE request to server
  std::future<RestResponse> DELETE(const std::string& uri, bool confirmable = false);

  // Ping the server
  std::future<RestResponse> PING();

  /**
   * Observe a ressource
   *
   * Returns an Observation object that reflects the latest received value of the observed ressource.
   * Additionally the Observation object allows the registration of a callback for incoming notifications.
   */
  Observation OBSERVE(const std::string& uri, bool confirmable = false);

 private:
  std::future<RestResponse> asFuture(Responses&& responses);

  ClientImpl& impl_;

  in_addr_t server_ip_;
  uint16_t server_port_;

  unsigned id_{0};
  std::map<unsigned, std::pair<std::promise<RestResponse>, Responses>> promises_;
};

}  // namespace CoAP

#endif  // __Client_h
