/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Client.h"

#include "Message.h"
#include "ClientImpl.h"

SETLOGLEVEL(LLWARNING)

namespace CoAP {

Client::Client(ClientImpl& impl, std::string server, uint16_t server_port)
    : impl_(impl)
    , server_ip_(NetUtils().ipFromHostname(server))
    , server_port_(server_port)
{
}

std::future<RestResponse> Client::GET(std::string uri, bool confirmable) {
  return asFuture(impl_.GET(server_ip_, server_port_, uri, confirmable ? CoAP::Type::Confirmable : CoAP::Type::NonConfirmable));
}

std::future<RestResponse> Client::PUT(std::string uri, std::string payload, bool confirmable) {
  return asFuture(impl_.PUT(server_ip_, server_port_, uri, payload, confirmable ? CoAP::Type::Confirmable : CoAP::Type::NonConfirmable));
}

std::future<RestResponse> Client::POST(std::string uri, std::string payload, bool confirmable) {
  return asFuture(impl_.POST(server_ip_, server_port_, uri, payload, confirmable ? CoAP::Type::Confirmable : CoAP::Type::NonConfirmable));
}

std::future<RestResponse> Client::DELETE(std::string uri, bool confirmable) {
  return asFuture(impl_.DELETE(server_ip_, server_port_, uri, confirmable ? CoAP::Type::Confirmable : CoAP::Type::NonConfirmable));
}

std::future<RestResponse> Client::PING() {
  return asFuture(impl_.PING(server_ip_, server_port_));
}

std::shared_ptr<Notifications> Client::OBSERVE(std::string uri, bool confirmable) {
  return impl_.OBSERVE(server_ip_, server_port_, uri, confirmable ? CoAP::Type::Confirmable : CoAP::Type::NonConfirmable);
}

std::future<RestResponse> Client::asFuture(const std::shared_ptr<Notifications>& responses) {
  auto id = ++id_;
  auto p = promises_.emplace(std::make_pair(id, std::make_pair(std::promise<RestResponse>(), responses)));

  // TODO: The callback must be registered along with the request that we don't miss a response due to race conditions
  p.first->second.second->subscribe([this, id](const RestResponse& r){
    auto it = this->promises_.find(id);
    if (this->promises_.end() == it) {
      ELOG << "Received unexpected response (" << r << ")\n";
      return;
    }
    it->second.first.set_value(r);
    this->promises_.erase(it);
  });

  auto & promise = p.first->second.first;
  return promise.get_future();
}
}  // namespace CoAP
