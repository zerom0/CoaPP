/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"
#include "RequestHandlerDispatcher.h"

#include "gtest/gtest.h"

TEST(ResponseHandlerDispatcher_GET, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // No URI is registered
  ASSERT_EQ(nullptr, rhd.getHandler(Path("/bar")));

  // Some other URI is registered
  rhd.onUri("/foo");
  ASSERT_EQ(nullptr, rhd.getHandler(Path("/bar")));

  // Now the URI exists and should be found
  rhd.onUri("/bar").onGet([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  auto handler = rhd.getHandler(Path("/bar"));
  ASSERT_NE(nullptr, handler);
  ASSERT_EQ(CoAP::Code::Content, handler->GET(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_PUT, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPut([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  auto handler = rhd.getHandler(Path("/bar"));
  ASSERT_NE(nullptr, handler);
  ASSERT_EQ(CoAP::Code::Created, handler->PUT(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_POST, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPost([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  auto handler = rhd.getHandler(Path("/bar"));
  ASSERT_NE(nullptr, handler);
  ASSERT_EQ(CoAP::Code::Changed, handler->POST(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_DELETE, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // Now the URI exists and should be found
  rhd.onUri("/bar").onDelete([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  auto handler = rhd.getHandler(Path("/bar"));
  ASSERT_NE(nullptr, handler);
  ASSERT_EQ(CoAP::Code::Deleted, handler->DELETE(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_OBSERVE, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  auto observable = std::make_shared<Observable<CoAP::RestResponse>>();
  auto notifiedValue = std::string();
  observable->subscribe([&notifiedValue](const CoAP::RestResponse& response){
    notifiedValue = response.payload();
  });

  // Now the URI exists and should be found
  rhd.onUri("/bar").onObserve([](const Path& uri, std::weak_ptr<Observable<CoAP::RestResponse>> notifications){
    notifications.lock()->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("else"));
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  auto handler = rhd.getHandler(Path("/bar"));
  ASSERT_NE(nullptr, handler);

  ASSERT_EQ(CoAP::Code::Content, handler->OBSERVE(Path("/bar"), observable).code());
  ASSERT_EQ("else", notifiedValue);
}


TEST(ResponseHandlerDispatcher_GET, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onDelete([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.GET(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_PUT, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onPost([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.PUT(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_POST, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onPut([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.POST(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_DELETE, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onGet([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.DELETE(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_OBSERVE, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;
  auto observable = std::make_shared<Observable<CoAP::RestResponse>>();

  handler.onDelete([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.OBSERVE(Path("/bar"), observable).code());
}
