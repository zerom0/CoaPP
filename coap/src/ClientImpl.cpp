/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ClientImpl.h"

#include "Messaging.h"

SETLOGLEVEL(LLWARNING);

namespace CoAP {

ClientImpl::~ClientImpl() {
  if (not notifications_.empty()) ELOG << "ClientImpl::notifications_ is not empty\n";
}

void ClientImpl::onMessage(const Message& msg_received, in_addr_t fromIP, uint16_t fromPort) {
  ILOG << "onMessage(): Message(" << msg_received
       << " payload=" << msg_received.payload().length() << " bytes)\n";

  std::lock_guard<std::mutex> lock(mutex_);

  auto notificationIt = notifications_.find(msg_received.token());

  if (notificationIt == notifications_.end()) {
    if (msg_received.hasObserveValue()) {
      // TODO: Send reset message to stop server from sending further notifications
    }
  }

  auto sp = notificationIt->second.lock();
  if (not sp) {
    if (msg_received.hasObserveValue()) {
      // TODO: Send reset message to stop server from sending further notifications
    }
  }

  sp->onNext(RestResponse()
                 .withSenderIP(fromIP)
                 .withSenderPort(fromPort)
                 .withCode(msg_received.code())
                 .withPayload(msg_received.payload()));
}

std::shared_ptr<Notifications> ClientImpl::GET(in_addr_t ip, uint16_t port, std::string uri, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "GET request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::GET, newToken(), uri));
}

std::shared_ptr<Notifications> ClientImpl::PUT(in_addr_t ip, uint16_t port, std::string uri, std::string payload, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "PUT request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::PUT, newToken(), uri, payload));
}

std::shared_ptr<Notifications> ClientImpl::POST(in_addr_t ip, uint16_t port, std::string uri, std::string payload, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "POST request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::POST, newToken(), uri, payload));
}

std::shared_ptr<Notifications> ClientImpl::DELETE(in_addr_t ip, uint16_t port, std::string uri, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "DELETE request with URI=" << uri << '\n';
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::DELETE, newToken(), uri));
}

std::shared_ptr<Notifications> ClientImpl::PING(in_addr_t ip, uint16_t port) {
  ILOG << "Sending ping request to the server\n";
  return sendRequest(ip, port, Message(Type::Confirmable, messageId_++, Code::Empty, newToken(), ""));
}

std::shared_ptr<Notifications> ClientImpl::OBSERVE(in_addr_t ip, uint16_t port, std::string uri, Type type) {
  ILOG << "Sending " << ((type == Type::Confirmable) ? "confirmable " : "") << "OBSERVATION request with URI=" << uri << '\n';
  auto msg = Message(type, messageId_++, CoAP::Code::GET, newToken(), uri);
  return sendRequest(ip, port, Message(type, messageId_++, CoAP::Code::GET, newToken(), uri).withObserveValue(0));
}

std::shared_ptr<Notifications> ClientImpl::sendRequest(in_addr_t ip, uint16_t port, Message msg) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto token = msg.token();
  auto notifications = std::make_shared<Notifications>([this, token](){
    this->notifications_.erase(token);
  });
  auto x = notifications_.emplace(token, notifications);
  // If there is already a request with the given token, we cannot send the request.
  if (not x.second) throw std::runtime_error("Sending request with already used token failed!");

  DLOG << "Sending " << ((msg.type() == Type::Confirmable) ? "confirmable " : "") << "message with msgID="
      << msg.messageId() << '\n';
  messaging_.sendMessage(ip, port, msg);
  return notifications;
}

}  // namespace CoAP