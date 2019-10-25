/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ConnectionMock.h"
#include "Messaging.h"
#include "Parameters.h"

#include "gtest/gtest.h"

#include <condition_variable>
#include <mutex>

class MClientTest: public testing::Test {
 public:
  MClientTest()
      : conn(new ConnectionMock()),
        time_(std::chrono::steady_clock::now()),
        messaging(conn, [this]() { return this->getTime(); }) {
  }

 protected:
  std::chrono::time_point<std::chrono::steady_clock> getTime() {
    return time_;
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

TEST_F(MClientTest, ResponseAssignedToRequest) {
  // GIVEN a client that sent a request
  ASSERT_EQ(0U, conn->sentMessages_.size());
  auto client = messaging.getMulticastClient(4711);
  auto observable = client.GET("/xyz");
  std::vector<CoAP::RestResponse> responses;
  observable->subscribe([&responses](const CoAP::RestResponse& response){
    responses.push_back(response);
  });
  ASSERT_EQ(1U, conn->sentMessages_.size());
  auto sentMsgId = conn->sentMessages_[0].messageId();
  auto sentMsgToken = conn->sentMessages_[0].token();
  // WHEN the client receives a response
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::NonConfirmable, sentMsgId, CoAP::Code::Content, sentMsgToken, "", "abc"));
  loopUntil([&responses](){ return !responses.empty(); });

  // THEN the response is related to the request
  ASSERT_EQ(1U, responses.size());
  auto response = responses.front();
  EXPECT_EQ(CoAP::Code::Content, response.code());
  EXPECT_EQ("abc", response.payload());
}

TEST_F(MClientTest, MultipleResponseAssignedToSameRequest) {
  // GIVEN a client that sent a request
  ASSERT_EQ(0U, conn->sentMessages_.size());
  auto client = messaging.getMulticastClient(4711);
  auto observable = client.GET("/xyz");
  std::vector<CoAP::RestResponse> responses;
  observable->subscribe([&responses](const CoAP::RestResponse& response){
    responses.push_back(response);
  });
  ASSERT_EQ(1U, conn->sentMessages_.size());
  auto sentMsgId = conn->sentMessages_[0].messageId();
  auto sentMsgToken = conn->sentMessages_[0].token();

  // WHEN the client receives multiple responses
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::NonConfirmable, sentMsgId, CoAP::Code::Content, sentMsgToken, "", "abc"));
  conn->addMessageToReceive(CoAP::Message(CoAP::Type::NonConfirmable, sentMsgId, CoAP::Code::Content, sentMsgToken, "", "def"));
  loopUntil([&responses](){ return responses.size() >= 2; });

  // THEN the responses are related to the request
  ASSERT_EQ(2U, responses.size());
  auto response = responses[0];
  EXPECT_EQ(CoAP::Code::Content, response.code());
  EXPECT_EQ("abc", response.payload());

  response = responses[1];
  EXPECT_EQ(CoAP::Code::Content, response.code());
  EXPECT_EQ("def", response.payload());
}
