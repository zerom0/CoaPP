/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MClient.h"

#include "ClientImpl.h"

CoAP::MClient::MClient(CoAP::ClientImpl& impl, uint16_t server_port)
  : impl_(impl)
  , multicast_ip_(htonl(0xE00001BB)) // "224.0.1.187"
  , server_port_(server_port)
{
}

CoAP::Responses CoAP::MClient::GET(const std::string& uri) {
  return impl_.GET(multicast_ip_, server_port_, uri, CoAP::Type::NonConfirmable);
}
