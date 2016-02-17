/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ClientImpl.h"

#include "Messaging.h"

SETLOGLEVEL(LLWARNING);

namespace CoAP {

ClientImpl::~ClientImpl() {
  if (not responses_.empty()) ELOG << "ClientImpl::responses_ is not empty\n";
}

void ClientImpl::onMessage(const Message& msg_received, in_addr_t fromIP, uint16_t fromPort) {
  ILOG << "onMessage(): Message(" << msg_received
       << " payload=" << msg_received.payload().length() << " bytes)\n";

  std::lock_guard<std::mutex> lock(mutex_);

  auto promiseIt = promises_.find(msg_received.token());
  auto responseIt = responses_.find(msg_received.token());

  if (promiseIt != promises_.end()) {
    promiseIt->second.set_value(RestResponse()
                             .withCode((CoAP::Code) msg_received.code())
                             .withPayload(msg_received.payload()));
    promises_.erase(promiseIt);
  }
  else if (responseIt != responses_.end()) {
    responseIt->second.emplace_back(RestResponse()
                                    .withSenderIP(fromIP)
                                    .withSenderPort(fromPort)
                                    .withCode((CoAP::Code) msg_received.code())
                                    .withPayload(msg_received.payload()));
    responseIt->second.callback();
  }
  else {
    ELOG << "Message with token=" << msg_received.token() << " is unexpected.\n";
  }
}

Responses ClientImpl::GET(in_addr_t ip, uint16_t port, const std::string& uri, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "GET request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::GET, newToken(), uri));
}

Responses ClientImpl::PUT(in_addr_t ip, uint16_t port, const std::string& uri, const std::string& payload, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "PUT request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::PUT, newToken(), uri, payload));
}

Responses ClientImpl::POST(in_addr_t ip,
                                           uint16_t port,
                                           const std::string& uri,
                                           const std::string& payload,
                                           Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "POST request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::POST, newToken(), uri, payload));
}

Responses ClientImpl::DELETE(in_addr_t ip, uint16_t port, const std::string& uri, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "DELETE request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::DELETE, newToken(), uri));
}

Responses ClientImpl::PING(in_addr_t ip, uint16_t port) {
  ILOG << "Sending ping request to the server\n";
  return sendRequest(ip, port, Message(Type::Confirmable, messageId_++, Code::Empty, newToken(), ""));
}

Responses ClientImpl::sendRequest(in_addr_t ip, uint16_t port, const Message& msg) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto token = msg.token();
  auto x = responses_.emplace(token, ResponsesImpl([this, token](){
    responses_.erase(token);
  }));
  // If there is already a request with the given token, we cannot send the request.
  if (not x.second) throw std::runtime_error("Sending request with already used token failed!");

  DLOG << "Sending " << ((msg.type() == Type::Confirmable) ? "confirmable " : "") << "message with msgID="
      << msg.messageId() << '\n';
  messaging_.sendMessage(ip, port, msg);
  return Responses(&x.first->second);
}

}  // namespace CoAP