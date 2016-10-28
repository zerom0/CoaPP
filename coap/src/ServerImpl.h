/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __ServerImpl_h
#define __ServerImpl_h

#include "RequestHandlerDispatcher.h"
#include "IConnection.h"
#include "Message.h"
#include "Notifications.h"

#include <map>
#include <memory>
#include <thread>

namespace CoAP {

class IRequestHandler;
class Messaging;
class RestResponse;
class Telegram;

class ServerImpl {
 public:
  ServerImpl(Messaging& messaging)
      : messaging_(messaging) {
  }

  RequestHandlerDispatcher& requestHandler() {
    return requestHandler_;
  }

  void onMessage(const Message& msg, in_addr_t fromIP, uint16_t fromPort);

  RestResponse onRequest(const Message& request, in_addr_t fromIP, uint16_t fromPort);

 private:
  void reply(in_addr_t ip, uint16_t port, Type type, MessageId messageId, uint64_t token, const RestResponse& response);

  RequestHandlerDispatcher requestHandler_;

  Messaging & messaging_;

  // observations are uniquely identified by the tuple <IP, Port, Token>
  std::map<std::tuple<in_addr_t, uint16_t, uint64_t>,std::shared_ptr<Notifications>> observations_;
  void createObservation(in_addr_t fromIP,
                         uint16_t fromPort,
                         Type requestType,
                         uint64_t token,
                         const Path& path);
  void deleteObservation(in_addr_t fromIP, uint16_t fromPort, uint64_t token);
};

}  // namespace CoAP

#endif  //__ServerImpl_h
