/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Messaging.h"
#include "ClientImpl.h"

#include "Connection.h"
#include "Logging.h"
#include "Message.h"
#include "Optional.h"
#include "Parameters.h"
#include "ServerImpl.h"

#include <cmath>

SETLOGLEVEL(LLWARNING)

namespace CoAP {

Messaging::Messaging(uint16_t port)
    : conn_(std::make_shared<Connection>()),
      timeProvider_(std::chrono::steady_clock::now),
      client_(new ClientImpl(*this)),
      server_(new ServerImpl(*this)) {
  auto conn = std::make_shared<Connection>();
  conn->open(port);
  conn_ = conn;
}

Messaging::Messaging(std::shared_ptr<IConnection> conn,
                     TimeProvider timeProvider)
    : conn_(conn),
      timeProvider_(timeProvider),
      client_(new ClientImpl(*this)),
      server_(new ServerImpl(*this)) {
}

Messaging::~Messaging() {
  // We need a destructor implemented in the .cpp file, otherwise the compiler
  // requires the full type of ClientImpl and ServerImpl in the automatically
  // generated destructor.

  if (loop_.joinable()) {
    loop_.join();
  }
}

namespace {
Optional<Message> messageFromTelegram(const Optional<Telegram>& telegram) {
  return telegram ? Message::fromBuffer(telegram.value().getMessage()) : Optional<Message>();
}
}

void Messaging::loopOnce() {
  resendUnacknowledged();
  onTelegram(conn_->get(std::chrono::milliseconds(100)));
}

void Messaging::loopStart() {
  loop_ = std::thread([=]() {
    while (not terminate_) {
      loopOnce();
    }
  });
}

void Messaging::loopStop() {
  terminate_ = true;
  //loop_.join();
}

void Messaging::onTelegram(const Optional<CoAP::Telegram>& telegram) {
  auto message = messageFromTelegram(telegram);
  if (message) onMessage(message.value(), telegram.value().getIP(), telegram.value().getPort());
}

void Messaging::onMessage(const Message& msg_received, in_addr_t fromIP, uint16_t fromPort) {
  switch (msg_received.type()) {
    case Type::Reset:
      onResetMessage(msg_received, fromIP, fromPort);
      break;

    case Type::Acknowledgement:
      onAcknowledgementMessage(msg_received);
      if (msg_received.code() == Code::Empty) return;
      if (msg_received.isRequestCode()) ELOG << "Received acknowledge with request code " << msg_received.code() << '\n';

      // ... fall through ...
    case Type::Confirmable:
    case Type::NonConfirmable:
      if (msg_received.isRequestCode()) {
        server_->onMessage(msg_received, fromIP, fromPort);
      } else {
        // When the client receives a confirmable response it can immediately acknowledge it
        if (msg_received.type() == Type::Confirmable) {
          DLOG << "Received confirmable request with msgID=" << msg_received.messageId()
               << " and token=" << msg_received.token() << '\n';

          acknowledge(fromIP, fromPort, msg_received.messageId());
        }

        client_->onMessage(msg_received, fromIP, fromPort);
      }
      break;

    default:
      ELOG << "Received unsupported message type " << static_cast<int>(msg_received.type()) << '\n';
  }
}

void Messaging::onAcknowledgementMessage(const Message& msg_received) {
  auto it = unacknowledged_.find(msg_received.messageId());
  if (it == unacknowledged_.end()) {
      ELOG << "Received unexpected acknowledge message with msgID=" << msg_received.messageId() << '\n';
    } else {
      DLOG << "Received acknowledge message with msgID=" << msg_received.messageId() << '\n';
      unacknowledged_.erase(it);
    }
}

void Messaging::onResetMessage(const Message& msg_received, in_addr_t fromIP, uint16_t fromPort) {
  auto it = unacknowledged_.find(msg_received.messageId());
  if (it != unacknowledged_.end()) {
      DLOG << "Received reset message for unacknowledged request with msgID=" << msg_received.messageId() << '\n';
      unacknowledged_.erase(it);
    }
  client_->onMessage(msg_received, fromIP, fromPort);
  server_->onMessage(msg_received, fromIP, fromPort);
}

RequestHandlerDispatcher& Messaging::requestHandler() {
  return server_->requestHandler();
}

Client Messaging::getClientFor(const char* server, uint16_t server_port) {
  return Client(*client_, server, server_port);
}

MClient Messaging::getMulticastClient(uint16_t server_port) {
  return MClient(*client_, server_port);
}

void Messaging::acknowledge(in_addr_t ip, uint16_t port, MessageId messageId) {
  DLOG << "Replying with empty acknowledge message with msgID=" << messageId << '\n';
  auto msg = Message(Type::Acknowledgement, messageId, Code::Empty, 0, "", "");
  sendMessage(ip, port, msg);
}

void Messaging::sendMessage(in_addr_t ip, uint16_t port, const Message& msg) {
  if (msg.type() == Type::Confirmable) {
    unacknowledged_.emplace(msg.messageId(), UnacknowledgedMessage(ip, port, msg, timeProvider_()));
  }

  conn_->send(Telegram(ip, port, msg.asBuffer()));
}


void Messaging::resendUnacknowledged() {
  const auto now = timeProvider_();

  std::vector<Message> expiredConfirmables;

  for (auto& unacknowledged : unacknowledged_) {
    auto& ua = unacknowledged.second;
    const auto factor = std::pow(2, ua.retransmits_ + 1) - 1;
    const auto nextTimeout = ua.sent_ + factor * ACK_TIMEOUT;

    if (nextTimeout <= now) {
      if (ua.retransmits_ < MAX_RETRANSMITS) {
        ++ua.retransmits_;
        ILOG << "Resending confirmable request with msgID=" << ua.msg_.messageId() << '\n';
        sendMessage(ua.ip_, ua.port_, ua.msg_);
      }
      else {
        ILOG << "Confirmable request with msgID=" << ua.msg_.messageId() << " expired\n";
        expiredConfirmables.emplace_back(Message(Type::Acknowledgement, ua.msg_.messageId(), Code::ServiceUnavailable, ua.msg_.token(), ""));
        server_->onMessage(Message(Type::Reset, ua.msg_.messageId(), ua.msg_.code(), ua.msg_.token(), ua.msg_.path()), ua.ip_, ua.port_);
      }
    }
  }

  // Handling the expired confirmable requests
  for (auto& expiredConfirmable : expiredConfirmables) {
    onMessage(expiredConfirmable, 0, 0);
  }
}

}  // namespace CoAP