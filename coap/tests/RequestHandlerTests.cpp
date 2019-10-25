/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"
#include "RequestHandlers.h"

#include "gtest/gtest.h"

TEST(RequestHandler_GET, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandler handler;

  handler.onGet([](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::Content, handler.GET(Path("/bar")).code());
}

TEST(RequestHandler_PUT, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandler handler;

  handler.onPut([](const Path&, const std::string&){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  ASSERT_EQ(CoAP::Code::Created, handler.PUT(Path("/bar"), "Something").code());
}

TEST(RequestHandler_POST, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandler handler;

  handler.onPost([](const Path&, const std::string&){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  ASSERT_EQ(CoAP::Code::Changed, handler.POST(Path("/bar"), "Something").code());
}

TEST(RequestHandler_DELETE, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandler handler;

  handler.onDelete([](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::Deleted, handler.DELETE(Path("/bar")).code());
}

TEST(RequestHandler_OBSERVE, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandler handler;

  auto observable = std::make_shared<Observable<CoAP::RestResponse>>();
  auto notifiedValue = std::string();
  observable->subscribe([&notifiedValue](const CoAP::RestResponse& response){
    notifiedValue = response.payload();
  });

  // Now the URI exists and should be found
  handler.onObserve([](const Path&, std::weak_ptr<Observable<CoAP::RestResponse>> notifications){
    notifications.lock()->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("else"));
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::Content, handler.OBSERVE(Path("/bar"), observable).code());
  ASSERT_EQ("else", notifiedValue);
}


TEST(RequestHandler_GET, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onDelete([](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.GET(Path("/bar")).code());
}

TEST(RequestHandler_PUT, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onPost([](const Path&, const std::string&){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.PUT(Path("/bar"), "Something").code());
}

TEST(RequestHandler_POST, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onPut([](const Path&, const std::string&){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.POST(Path("/bar"), "Something").code());
}

TEST(RequestHandler_DELETE, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;

  handler.onGet([](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.DELETE(Path("/bar")).code());
}

TEST(RequestHandler_OBSERVE, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandler handler;
  auto observable = std::make_shared<Observable<CoAP::RestResponse>>();

  handler.onDelete([](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, handler.OBSERVE(Path("/bar"), observable).code());
}
