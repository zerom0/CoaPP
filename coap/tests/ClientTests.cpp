/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ConnectionMock.h"
#include "Messaging.h"
#include "Parameters.h"

#include "gtest/gtest.h"

#include <condition_variable>
#include <mutex>

class ClientTest: public testing::Test {
 public:
  ClientTest()
      : conn(new ConnectionMock()),
        time_(std::chrono::steady_clock::now()),
        messaging(conn, [this]() { return this->getTime(); }) {
  }

 protected:
  std::chrono::time_point<std::chrono::steady_clock> getTime() {
    return time_;
  }

  void loopUntil(const std::future<CoAP::RestResponse>& response) {
    loopUntil([&response]() {
      return response.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    });
  }

  void loopUntil(std::function<bool()> pre) {
    while (!pre())
      messaging.loopOnce();
  }

  void advance(const std::chrono::milliseconds& offset) {
    time_ += offset;
  }

  std::shared_ptr<ConnectionMock> conn;
  std::chrono::time_point<std::chrono::steady_clock> time_;
  CoAP::Messaging messaging;
};

TEST_F(ClientTest, TimeStandsStill) {
  auto time = getTime();
  ASSERT_EQ(time, getTime());
}

TEST_F(ClientTest, TimeCanBeAdvanced) {
  auto time = getTime();
  advance(std::chrono::milliseconds(234));
  EXPECT_LT(time, getTime());
  EXPECT_EQ(std::chrono::milliseconds(234), getTime() - time);
}

TEST_F(ClientTest, EachRequestIncreasesMessageId) {
  // GIVEN a Client

  conn->addMessageToReceive(
    CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::Content, 0, ""));
  conn->addMessageToReceive(
    CoAP::Message(CoAP::Type::NonConfirmable, 1, CoAP::Code::Content, 0, ""));
  conn->addMessageToReceive(
    CoAP::Message(CoAP::Type::NonConfirmable, 2, CoAP::Code::Content, 0, ""));
  conn->addMessageToReceive(
    CoAP::Message(CoAP::Type::NonConfirmable, 3, CoAP::Code::Content, 0, ""));
  conn->addMessageToReceive(
    CoAP::Message(CoAP::Type::NonConfirmable, 4, CoAP::Code::Content, 0, ""));

  // WHEN the client sends requests
  auto client = messaging.getClientFor("localhost", 4711);
  auto r1 = client.GET("/xyz");
  auto r2 = client.PUT("/xyz", "abc");
  auto r3 = client.POST("/xyz", "abc");
  auto r4 = client.DELETE("/xyz");
  auto r5 = client.GET("/xyz");

  // THEN it uses increasing message ids
  ASSERT_EQ(5U, conn->sentMessages_.size());
  EXPECT_EQ(0, conn->sentMessages_[0].messageId());
  EXPECT_EQ(1, conn->sentMessages_[1].messageId());
  EXPECT_EQ(2, conn->sentMessages_[2].messageId());
  EXPECT_EQ(3, conn->sentMessages_[3].messageId());
  EXPECT_EQ(4, conn->sentMessages_[4].messageId());
}

TEST_F(ClientTest, ConfirmableResponseCausesAcknowledge) {
  // GIVEN a client

  // WHEN the client receives a confirmable response
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::Acknowledgement, 0, CoAP::Code::Empty, 0, ""));
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::Confirmable, 23, CoAP::Code::Content, 0, ""));

  ASSERT_EQ(0U, conn->sentMessages_.size());
  CoAP::Client client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  ASSERT_EQ(1U, conn->sentMessages_.size());
  loopUntil(response);
  response.get();

  ASSERT_EQ(2U, conn->sentMessages_.size());

  EXPECT_EQ(0, conn->sentMessages_[0].messageId());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());

  // THEN it acknowledges the reception
  EXPECT_EQ(23, conn->sentMessages_[1].messageId());
  EXPECT_EQ(CoAP::Type::Acknowledgement, conn->sentMessages_[1].type());
}

TEST_F(ClientTest, ConfirmableRequestResentOnlyMaxRetransmits) {
  // GIVEN a confirmable message sent by the client Client
  CoAP::Client client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  for (auto i = 0; i < 1000; ++i) {
    advance(CoAP::ACK_TIMEOUT);
    messaging.loopOnce();
  }

  // THEN it resends the request max MAX_RETRANSMITS times
  ASSERT_EQ(1U + CoAP::MAX_RETRANSMITS, conn->sentMessages_.size());
  // AND marks an error in the response
  ASSERT_EQ(std::future_status::ready, response.wait_for(std::chrono::microseconds(1)));
  ASSERT_EQ(CoAP::Code::ServiceUnavailable, response.get().code());
}

TEST_F(ClientTest, TimeframeForFirstRetransmit) {
  // GIVEN a confirmable message sent by the client Client
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  // THEN it sends no request before ACK_TIMEOUT
  advance(CoAP::ACK_TIMEOUT - std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // BUT before ACK_RANDOM_NUMBER * ACK_TIMEOUT
  advance((CoAP::ACK_TIMEOUT * (CoAP::ACK_RANDOM_NUMBER - 100)) / 100 + 2 * std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(2U, conn->sentMessages_.size());
}

TEST_F(ClientTest, TimeframeForSecondRetransmit) {
  // GIVEN a confirmable message sent by the client Client
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  // THEN it sends no second request before 3*ACK_TIMEOUT
  advance((3*CoAP::ACK_TIMEOUT) - std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(2U, conn->sentMessages_.size());

  // BUT before 3 * ACK_RANDOM_NUMBER * ACK_TIMEOUT
  advance(3 * (CoAP::ACK_TIMEOUT * (CoAP::ACK_RANDOM_NUMBER - 100)) / 100 + 2 * std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(3U, conn->sentMessages_.size());
}

TEST_F(ClientTest, TimeframeForThirdRetransmit) {
  // GIVEN a confirmable message sent by the client Client
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  // THEN it sends no third request before 7*ACK_TIMEOUT
  advance((1*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((2*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((4*CoAP::ACK_TIMEOUT) - std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(3U, conn->sentMessages_.size());

  // BUT before 7 * ACK_RANDOM_NUMBER * ACK_TIMEOUT
  advance(7 * (CoAP::ACK_TIMEOUT * (CoAP::ACK_RANDOM_NUMBER - 100)) / 100 + 2 * std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(4U, conn->sentMessages_.size());
}

TEST_F(ClientTest, TimeframeForFourthRetransmit) {
  // GIVEN a confirmable message sent by the client Client
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  // THEN it sends no fourth request before 15*ACK_TIMEOUT
  advance((1*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((2*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((4*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((8*CoAP::ACK_TIMEOUT) - std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(4U, conn->sentMessages_.size());

  // BUT before 15 * ACK_RANDOM_NUMBER * ACK_TIMEOUT
  advance(15 * (CoAP::ACK_TIMEOUT * (CoAP::ACK_RANDOM_NUMBER - 100)) / 100 + 2 * std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(5U, conn->sentMessages_.size());
}

TEST_F(ClientTest, TimeoutExpires) {
  // GIVEN a confirmable message sent by the client Client
  CoAP::Client client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the client receives no acknowledge
  // THEN it does not timeout the request before 31*ACK_TIMEOUT
  advance((1*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((2*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((4*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((8*CoAP::ACK_TIMEOUT));
  messaging.loopOnce();
  advance((16*CoAP::ACK_TIMEOUT) - std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(1U + CoAP::MAX_RETRANSMITS, conn->sentMessages_.size());
  ASSERT_EQ(std::future_status::timeout, response.wait_for(std::chrono::microseconds(1)));

  // BUT before 31 * ACK_RANDOM_NUMBER * ACK_TIMEOUT
  advance(31 * (CoAP::ACK_TIMEOUT * (CoAP::ACK_RANDOM_NUMBER - 100)) / 100 + 2 * std::chrono::milliseconds(10));
  messaging.loopOnce();
  ASSERT_EQ(1U + CoAP::MAX_RETRANSMITS, conn->sentMessages_.size());

  // AND marks an error in the response
  ASSERT_EQ(std::future_status::ready, response.wait_for(std::chrono::microseconds(1)));
  ASSERT_EQ(CoAP::Code::ServiceUnavailable, response.get().code());

}

TEST_F(ClientTest, ConfirmableRequestResentUntilAcknowledge) {
  // GIVEN a Client

  // WHEN the client sends a confirmable request but receives no acknowledge
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();

  // THEN it resends the request after some time
  ASSERT_EQ(1U, conn->sentMessages_.size());
  advance(std::chrono::milliseconds(2000));
  messaging.loopOnce();
  ASSERT_EQ(2U, conn->sentMessages_.size());

  // but no more messages after the acknowledge is received
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::Acknowledgement, 0, CoAP::Code::Empty, 0, ""));
  messaging.loopOnce();
  advance(std::chrono::milliseconds(2000));
  messaging.loopOnce();
  ASSERT_EQ(2U, conn->sentMessages_.size());
}

TEST_F(ClientTest, ConfirmableRequestAnsweredWithRST) {
  // GIVEN a Client that sends a request
  auto client = messaging.getClientFor("localhost", 4711);
  auto response = client.GET("/xyz", true);
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());

  // WHEN the server responds with a RST
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::Reset, 0, CoAP::Code::Empty, 0, ""));
  messaging.loopOnce();


  // THEN the client must remove its request and not send more requests because the ACK was not received
  ASSERT_EQ(1U, conn->sentMessages_.size());
  advance(std::chrono::milliseconds(2000));
  messaging.loopOnce();
  ASSERT_EQ(1U, conn->sentMessages_.size());
}