/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ConnectionMock.h"
#include "Messaging.h"
#include "ServerImpl.h"
#include "Path.h"

#include "gtest/gtest.h"

//    Client            Server
//      |       NON       |   NonConfirmable request
//      |---------------->|
//      |                 |
//      |       NON       |   NonConfirmable answer for request
//      |<----------------|
TEST(ServerImpl_onMessage, NoAcknowledgeForNonConfirmableMessage) {
  // GIVEN a NonConfirmable message
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
          .onGet([](const Path& path){
            return CoAP::RestResponse().withCode(CoAP::Code::Content);
          });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/");

  // WHEN the NonConfirmable message is sent to the server
  srv.onMessage(msg, 0, 0);

  // THEN we only get the answer back, but no acknowledge for reception of the message
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::NonConfirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[0].responseCode());
}

//    Client            Server
//      |       CON       |   Confirmable request
//      |---------------->|
//      |                 |
//      |       ACK       |   Acknowledge for reception of request
//      |<----------------|
//      |                 |
//      |       CON       |   Confirmable answer for request
//      |<----------------|
//      :                 :
//      :       ACK       :   Client acknowledge is not tested here
//      :- - - - - - - - >:
TEST(ServerImpl_onMessage, AcknowledgeForConfirmableMessage) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
          .onGet([](const Path& path){
            return CoAP::RestResponse().withCode(CoAP::Code::Content);
          }, true);
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/");

  // WHEN
  srv.onMessage(msg, 0, 0);

  // THEN
  ASSERT_EQ(2, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Acknowledgement, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[1].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[1].responseCode());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForGET) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onGet([](const Path& path){
        return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Hello");
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[0].responseCode());
  EXPECT_EQ("Hello", conn->sentMessages_[0].payload());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForPUT) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onPut([](const Path& path, const std::string& payload){
        return CoAP::RestResponse().withCode(CoAP::Code::Created);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::PUT, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Created, conn->sentMessages_[0].responseCode());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForPOST) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onPost([](const Path& path, const std::string& payload){
        return CoAP::RestResponse().withCode(CoAP::Code::Changed);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::POST, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Changed, conn->sentMessages_[0].responseCode());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForDELETE) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onDelete([](const Path& path){
        return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::DELETE, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Deleted, conn->sentMessages_[0].responseCode());
}

TEST(ServerImpl_onRequest, DelegatesGetRequestAndReceivesResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto getCalled = 0;
  srv.requestHandler()
      .onUri("/*")
          .onGet([&getCalled](const Path& path){
            ++getCalled;
            return CoAP::RestResponse().withCode(CoAP::Code::Content);
          });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/some/where");

  // WHEN NonConfirmable message with GET request is sent to the Server
  auto reply = srv.getServer().onRequest(msg, 0, 0);

  // THEN RequestHandler GET is called with the message and an reply is sent
  EXPECT_EQ(1, getCalled);

  EXPECT_EQ(CoAP::Code::Content, reply.code());
}

TEST(ServerImpl_onRequest, DelegatesPutRequestAndReceivesResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto putCalled = 0;
  srv.requestHandler()
      .onUri("/*")
          .onPut([&putCalled](const Path& path, std::string payload){
            ++putCalled;
            return CoAP::RestResponse().withCode(CoAP::Code::Changed);
          });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::PUT, 0, "/some/where");

  // WHEN NonConfirmable message with PUT request is sent to the Server
  auto reply = srv.getServer().onRequest(msg, 0, 0);

  // THEN RequestHandler PUT is called with the message and an reply is sent
  EXPECT_EQ(1, putCalled);

  EXPECT_EQ(CoAP::Code::Changed, reply.code());
}

TEST(ServerImpl_onRequest, DelegatesPostRequestAndReceivesResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto postCalled = 0;
  srv.requestHandler()
      .onUri("/*")
          .onPost([&postCalled](const Path& path, std::string payload){
            ++postCalled;
            return CoAP::RestResponse().withCode(CoAP::Code::Created);
          });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::POST, 0, "/some/where");

  // WHEN NonConfirmable message with POST request is sent to the Server
  auto reply = srv.getServer().onRequest(msg, 0, 0);

  // THEN RequestHandler POST is called with the message and an reply is sent
  EXPECT_EQ(1, postCalled);

  EXPECT_EQ(CoAP::Code::Created, reply.code());
}

TEST(ServerImpl_onRequest, DelegatesDeleteRequestAndReceivesResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto deleteCalled = 0;
  srv.requestHandler()
      .onUri("/*")
        .onDelete([&deleteCalled](const Path& path){
          ++deleteCalled;
          return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
        });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::DELETE, 0, "/some/where");

  // WHEN NonConfirmable message with DELETE request is sent to the Server
  auto reply = srv.getServer().onRequest(msg, 0, 0);

  // THEN RequestHandler DELETE is called with the message and an reply is sent
  EXPECT_EQ(1, deleteCalled);

  EXPECT_EQ(CoAP::Code::Deleted, reply.code());
}

TEST(ServerImpl_onMessage, EmptyRequestCausesPingResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);

  // WHEN
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::Empty, 0, "");
  srv.onMessage(msg, 0, 0);

  // THEN
  ASSERT_EQ(1, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Reset, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Empty, conn->sentMessages_[0].responseCode());
}