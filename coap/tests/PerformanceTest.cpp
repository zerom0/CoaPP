/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include <Messaging.h>
#include <IConnection.h>
#include <list>
#include <RequestHandlers.h>

using CoAP::Telegram;

class LoopbackConnection : public CoAP::IConnection {
  std::list<Telegram> telegrams_;

 public:
  void send(Telegram &&telegram) override {
    telegrams_.emplace_back(telegram);
  }

  Optional<Telegram> get(std::chrono::milliseconds) override {
    if (telegrams_.empty()) return Optional<Telegram>();

    auto telegram = std::move(telegrams_.front());
    telegrams_.pop_front();
    return telegram;
  }
};

TEST(Performance, Test) {
  CoAP::Messaging messaging(std::make_shared<LoopbackConnection>());

  messaging.requestHandler()
      .onUri("hello")
          .onGet([](const Path&){
            return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("world");
          });

  auto client = messaging.getClientFor("127.0.0.1");

  for (int i = 0; i < 100'000; ++i) {
    auto response = client.GET("hello");

    while (response.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) messaging.loopOnce();

    auto r = response.get();
  }
//  EXPECT_EQ(CoAP::Code::Content, r.code());
//  EXPECT_EQ("world", r.payload());
}
