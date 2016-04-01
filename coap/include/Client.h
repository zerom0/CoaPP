/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Client_h
#define __Client_h

#include "Notifications.h"
#include "Responses.h"
#include "RestResponse.h"

#include <string>
#include <future>
#include <netinet/in.h>
#include <map>

namespace CoAP {

class ClientImpl;
class IRequestHandler;

/*
 * Class: Client
 *
 * Instance of CoAP client connected to a CoAP server. It provides users with the
 * ability to send REST requests in an asynchronous way.
 *
 * Clients can be created with <IMessaging::getClientFor()>.
 */
class Client {
 public:
  Client(ClientImpl& impl, const std::string& server, uint16_t server_port);

  /*
   * Method: GET
   *
   * Sends a GET request to the CoAP server in order to read a resource.
   *
   * Parameters:
   *    uri         - URI of the ressource to be returned
   *    confirmable - true: confirmable messaging (default) /
   *                  false: nonconfirmable messaging
   *
   * Returns:
   *    A future with the <RestResponse>, once it will be received.
   */
  std::future<RestResponse> GET(const std::string& uri, bool confirmable = false);

  /*
   * Method: PUT
   *
   * Sends a PUT request to the CoAP server in order to update a resource.
   *
   * Parameters:
   *    uri         - URI of the ressource to be updated
   *    payload     - Payload of the PUT request being sent
   *    confirmable - true: confirmable messaging (default) /
   *                  false: nonconfirmable messaging
   *
   * Returns:
   *    A future with the <RestResponse>, once it will be received.
   */
  std::future<RestResponse> PUT(const std::string& uri, const std::string& payload, bool confirmable = false);

  /*
   * Method: POST
   *
   * Sends a POST request to the CoAP server in order to create a resource.
   *
   * Parameters:
   *    uri         - URI of the ressource to be created
   *    payload     - Payload of the PUT request being sent
   *    confirmable - true: confirmable messaging (default) /
   *                  false: nonconfirmable messaging
   *
   * Returns:
   *    A future with the <RestResponse>, once it will be received.
   */
  std::future<RestResponse> POST(const std::string& uri, const std::string& payload, bool confirmable = false);

  /*
   * Method: DELETE
   *
   * Sends a DELETE request to the CoAP server in order to delete a resource.
   *
   * Parameters:
   *    uri         - URI of the ressource to be deleted
   *    confirmable - true: confirmable messaging (default) /
   *                  false: nonconfirmable messaging
   *
   * Returns:
   *    A future with the <RestResponse>, once it will be received.
   */
  std::future<RestResponse> DELETE(const std::string& uri, bool confirmable = false);

  /*
   * Method: PING
   *
   * Sends a PING request to the CoAP server to check if it is available.
   *
   * Returns:
   *    A future with the <RestResponse>, once it will be received.
   */
  std::future<RestResponse> PING();

  /**
   * Method: OBSERVE
   *
   * Sends an Observation request to the CoAP server to receive regular updates
   * of the resource.
   *
   * Parameters:
   *    uri         - URI of the resource to be observed
   *    confirmable - true: confirmable messaging (default) /
   *                  false: nonconfirmable messaging
   *
   * Returns:
   *    Notifications with updated representations of the resource.
   */
  std::shared_ptr<Notifications> OBSERVE(const std::string &uri, bool confirmable = false);

 private:
  std::future<RestResponse> asFuture(const std::shared_ptr<Notifications>& responses);

  ClientImpl& impl_;

  in_addr_t server_ip_;
  uint16_t server_port_;

  unsigned id_{0};
  std::map<unsigned, std::pair<std::promise<RestResponse>, std::shared_ptr<Notifications>>> promises_;
};

}  // namespace CoAP

#endif  // __Client_h
