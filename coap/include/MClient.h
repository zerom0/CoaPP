/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __MClient_h
#define __MClient_h

#include "Notifications.h"

#include <netinet/in.h>

namespace CoAP {

class ClientImpl;

class MClient {
 public:
  MClient(ClientImpl& impl, uint16_t server_port);

  /**
   * Send a nonconfirmable GET request to multicast address
   *
   * Returns a collection of RestResponses along with their server addresses, the collection will
   * be extended with incoming responses over time. When the Responses object is being closed or
   * destroyed further incoming responses will be ignored.
   */
  std::shared_ptr<Notifications> GET(const std::string& uri);

 private:
  ClientImpl& impl_;

  in_addr_t multicast_ip_;
  uint16_t server_port_;
};

}  // namespace CoAP

#endif //__MClient_h
