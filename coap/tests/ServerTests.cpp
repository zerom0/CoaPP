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
          .onGet([](const Path&){
            return CoAP::RestResponse().withCode(CoAP::Code::Content);
          });
  auto msg = CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/");

  // WHEN the NonConfirmable message is sent to the server
  srv.onMessage(msg, 0, 0);

  // THEN we only get the answer back, but no acknowledge for reception of the message
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::NonConfirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[0].code());
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
          .onGet([](const Path&){
            return CoAP::RestResponse().withCode(CoAP::Code::Content);
          }, true);
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/");

  // WHEN
  srv.onMessage(msg, 0, 0);

  // THEN
  ASSERT_EQ(2U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Acknowledgement, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[1].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[1].code());
}

TEST(ServerImpl_onMessage, NoAcknowledgeForConfirmableMessageOnInvalidRessource) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/abc")
      .onGet([](const Path&){
        return CoAP::RestResponse().withCode(CoAP::Code::Content);
      }, true);
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/xyz");

  // WHEN
  srv.onMessage(msg, 0, 0);

  // THEN
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::NotFound, conn->sentMessages_[0].code());
}


TEST(ServerImpl_onRequest, PiggybackedResponseForGET) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onGet([](const Path&){
        return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Hello");
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Content, conn->sentMessages_[0].code());
  EXPECT_EQ("Hello", conn->sentMessages_[0].payload());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForPUT) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onPut([](const Path&, const std::string&){
        return CoAP::RestResponse().withCode(CoAP::Code::Created);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::PUT, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Created, conn->sentMessages_[0].code());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForPOST) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onPost([](const Path&, const std::string&){
        return CoAP::RestResponse().withCode(CoAP::Code::Changed);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::POST, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Changed, conn->sentMessages_[0].code());
}

TEST(ServerImpl_onRequest, PiggybackedResponseForDELETE) {
  // GIVEN a Server with a response handler
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  srv.requestHandler()
      .onUri("/")
      .onDelete([](const Path&){
        return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
      });

  // WHEN it receives a confirmable message with a GET request
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::DELETE, 0, "/");
  srv.onMessage(msg, 0, 0);

  // THEN it sends an ACK message with a piggybacked response
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Confirmable, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Deleted, conn->sentMessages_[0].code());
}

TEST(ServerImpl_onRequest, DelegatesGetRequestAndReceivesResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto getCalled = 0;
  srv.requestHandler()
      .onUri("/*")
          .onGet([&getCalled](const Path&){
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
          .onPut([&putCalled](const Path&, std::string){
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
          .onPost([&postCalled](const Path&, std::string){
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
        .onDelete([&deleteCalled](const Path&){
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

TEST(ServerImpl_onRequest, DelegatesObserveRequestAndReceivesNotification) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto observeCalled = 0;
  srv.requestHandler()
      .onUri("/*")
        .onGet([](const Path&){
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Get");
        })
        .onObserve([&observeCalled](const Path&, std::weak_ptr<CoAP::Notifications> observer){
          ++observeCalled;
          observer.lock()->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Notification"));
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Observe");
        });
  auto msg =
      CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/some/where")
      .withObserveValue(0);

  // WHEN NonConfirmable message with GET request is sent with OBSERVE=0 to the Server
  auto reply = srv.getServer().onRequest(msg, 0, 0);

  // THEN RequestHandler OBSERVE is called with the message and an reply is sent
  EXPECT_EQ(1, observeCalled);

  EXPECT_EQ(CoAP::Code::Content, reply.code());
  EXPECT_EQ("Observe", reply.payload());

  // AND in this example the OBSERVE handler sends also a Notification.
  EXPECT_EQ(1U, conn->sentMessages_.size());
  auto firstMessage = conn->sentMessages_.front();
  EXPECT_EQ("Notification", firstMessage.payload());
}

TEST(ServerImpl_onRequest, DelegatesObserveRequestAndCancelsNotifications) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto observeCalled = 0;
  std::weak_ptr<CoAP::Notifications> notifications;
  srv.requestHandler()
      .onUri("/*")
        .onGet([](const Path&){
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Get");
        })
        .onObserve([&observeCalled, &notifications](const Path&, std::weak_ptr<CoAP::Notifications> observer){
          ++observeCalled;
          notifications = observer;
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Observe");
        });
  auto observeMsg =
      CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/some/where")
          .withObserveValue(0);
  auto unobserveMsg =
      CoAP::Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/some/where")
          .withObserveValue(1);

  // WHEN NonConfirmable message with GET request is sent with OBSERVE=1 to the Server
  auto observeReply = srv.getServer().onRequest(observeMsg, 0, 0);
  notifications.lock()->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Notification"));
  auto unobserveReply = srv.getServer().onRequest(unobserveMsg, 0, 0);

  // THEN the notification observer is canceled
  EXPECT_EQ(true, notifications.expired());
}

TEST(ServerImpl_onRequest, RejectedNotificationCancelsObservation) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);
  auto observeCalled = 0;
  std::weak_ptr<CoAP::Notifications> notifications;
  srv.requestHandler()
      .onUri("/*")
        .onGet([](const Path&){
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Get");
        })
        .onObserve([&observeCalled, &notifications](const Path&, std::weak_ptr<CoAP::Notifications> observer){
          ++observeCalled;
          notifications = observer;
          return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Observe");
        });
  auto observeMsg =
      CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::GET, 0, "/some/where")
          .withObserveValue(0);

  // WHEN NonConfirmable message with GET request is sent to the Server and the first
  //      notification is answered with Reset
  auto observeReply = srv.getServer().onRequest(observeMsg, 0, 0);
  notifications.lock()->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("Notification"));
  srv.getServer().onMessage(CoAP::Message(CoAP::Type::Reset, 0, CoAP::Code::GET, 0, "/some/where"), 0, 0);

  // THEN the notification observer is canceled
  EXPECT_EQ(true, notifications.expired());
}

TEST(ServerImpl_onMessage, EmptyRequestCausesPingResponse) {
  // GIVEN
  auto conn = std::make_shared<ConnectionMock>();
  CoAP::Messaging srv(conn);

  // WHEN
  auto msg = CoAP::Message(CoAP::Type::Confirmable, 0, CoAP::Code::Empty, 0, "");
  srv.onMessage(msg, 0, 0);

  // THEN
  ASSERT_EQ(1U, conn->sentMessages_.size());
  EXPECT_EQ(CoAP::Type::Reset, conn->sentMessages_[0].type());
  EXPECT_EQ(CoAP::Code::Empty, conn->sentMessages_[0].code());
}