/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Messaging_h
#define __Messaging_h

#include "IMessaging.h"

#include "Client.h"
#include "Message.h"

#include <chrono>
#include <functional>
#include <map>
#include <thread>

namespace CoAP {

class ClientImpl;
class RequestHandlerDispatcher;
class IConnection;
class IRequestHandler;
class ServerImpl;
class Telegram;


class Messaging : public IMessaging {
 public:
  /// Creates a Messaging object listening on the specified port
  explicit Messaging(uint16_t port);

  explicit Messaging(std::shared_ptr<IConnection> conn,
                     std::function<std::chrono::time_point<std::chrono::steady_clock>()> timeProvider = std::chrono::steady_clock::now);

  virtual ~Messaging();

  void loopOnce() override;

  void loopStart() override;

  void loopStop() override;

  RequestHandlerDispatcher& requestHandler() override;

  Client getClientFor(const char* server, uint16_t server_port = 5683) override ;

  MClient getMulticastClient(uint16_t server_port) override;

  void acknowledge(in_addr_t ip, uint16_t port, MessageId messageId);

  void sendMessage(in_addr_t ip, uint16_t port, const Message& msg);

  void onMessage(const Message& msg, in_addr_t fromIP, uint16_t fromPort);

  ServerImpl& getServer() { return *server_; }

 private:
  void resendUnacknowledged();
  void onTelegram(const Optional<Telegram>& anOptional);
  void onResetMessage(const Message& msg_received, in_addr_t fromIP, uint16_t fromPort);
  void onAcknowledgementMessage(const Message& msg_received);

  std::function<std::chrono::time_point<std::chrono::steady_clock>()> timeProvider_;

  struct Unacknowledged {
    Unacknowledged(in_addr_t ip, uint16_t port, const Message& msg, std::chrono::time_point<std::chrono::steady_clock> sent)
        : ip_(ip), port_(port), msg_(msg), sent_(sent) {
    }

    in_addr_t ip_{0};
    uint16_t port_{0};
    Message msg_;
    uint32_t retransmits_{0};
    std::chrono::time_point<std::chrono::steady_clock> sent_;
  };
  std::map<MessageId, Unacknowledged> unacknowledged_;

  std::shared_ptr<IConnection> conn_;

  std::unique_ptr<ClientImpl> client_;
  std::unique_ptr<ServerImpl> server_;

  std::thread loop_;

  bool terminate_{false};
};

}  // namespace CoAP

#endif  // __Messaging_h
