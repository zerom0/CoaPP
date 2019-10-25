/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ConnectionMock_h
#define __ConnectionMock_h

#include "IConnection.h"
#include "Logging.h"
#include "Message.h"

#include <cassert>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

SETLOGLEVEL(LLWARNING)

class ConnectionMock : public CoAP::IConnection {
 public:
  virtual ~ConnectionMock() = default;

  virtual void send(CoAP::Telegram&& telegram) override {
    ILOG << "send(): Message(" << CoAP::Message::fromBuffer(telegram.getMessage()) << ")\n";
    sentMessages_.push_back(CoAP::Message::fromBuffer(telegram.getMessage()));
  }

  virtual Optional<CoAP::Telegram> get(std::chrono::milliseconds) override {
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, std::chrono::microseconds(1), [&] { return messagesReceived_ < messagesToReceive_.size(); })) {
      ILOG << "get(): Message(" << CoAP::Message::fromBuffer(messagesToReceive_[messagesReceived_]) << ")\n";
      return Optional<CoAP::Telegram>(0, 0, static_cast<std::vector<uint8_t>>(messagesToReceive_[messagesReceived_++]));
    } else {
      return Optional<CoAP::Telegram>();
    }
  }

  void addMessageToReceive(const CoAP::Message& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messagesToReceive_.push_back(message.asBuffer());
    cv_.notify_all();
  }

  std::vector<CoAP::Message> sentMessages_;

 private:
  std::condition_variable cv_;
  std::mutex mutex_;
  std::vector<CoAP::Message::Buffer> messagesToReceive_;
  size_t messagesReceived_{0};
};

#endif //__ConnectionMock_h
