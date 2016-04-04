/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MClient.h"

#include "ClientImpl.h"

namespace CoAP {

MClient::MClient(ClientImpl &impl, uint16_t server_port)
    : impl_(impl), multicast_ip_(htonl(0xE00001BB)) // "224.0.1.187"
    , server_port_(server_port) {
}

std::shared_ptr<Notifications> MClient::GET(std::string uri) {
  return impl_.GET(multicast_ip_, server_port_, uri, Type::NonConfirmable);
}

}  // CoAP