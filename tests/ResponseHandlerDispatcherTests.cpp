/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"
#include "RequestHandlerDispatcher.h"

#include "gtest/gtest.h"

TEST(ResponseHandlerDispatcher_GET, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // No URI is registered
  ASSERT_EQ(CoAP::Code::NotFound, rhd.GET(Path("/bar")).code());

  // Some other URI is registered
  rhd.onUri("/foo");
  ASSERT_EQ(CoAP::Code::NotFound, rhd.GET(Path("/bar")).code());

  // Now the URI exists and should be found
  rhd.onUri("/bar").onGet([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::Content, rhd.GET(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_PUT, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // No URI is registered
  ASSERT_EQ(CoAP::Code::NotFound, rhd.PUT(Path("/bar"), "Something").code());

  // Some other URI is registered
  rhd.onUri("/foo");
  ASSERT_EQ(CoAP::Code::NotFound, rhd.PUT(Path("/bar"), "Something").code());

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPut([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  ASSERT_EQ(CoAP::Code::Created, rhd.PUT(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_POST, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // No URI is registered
  ASSERT_EQ(CoAP::Code::NotFound, rhd.POST(Path("/bar"), "Something").code());

  // Some other URI is registered
  rhd.onUri("/foo");
  ASSERT_EQ(CoAP::Code::NotFound, rhd.POST(Path("/bar"), "Something").code());

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPost([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  ASSERT_EQ(CoAP::Code::Changed, rhd.POST(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_DELETE, ReturnsNotFoundForUnknownURI) {
  CoAP::RequestHandlerDispatcher rhd;

  // No URI is registered
  ASSERT_EQ(CoAP::Code::NotFound, rhd.DELETE(Path("/bar")).code());

  // Some other URI is registered
  rhd.onUri("/foo");
  ASSERT_EQ(CoAP::Code::NotFound, rhd.DELETE(Path("/bar")).code());

  // Now the URI exists and should be found
  rhd.onUri("/bar").onDelete([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::Deleted, rhd.DELETE(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_GET, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandlerDispatcher rhd;

  rhd.onUri("/bar").onDelete([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, rhd.GET(Path("/bar")).code());
}

TEST(ResponseHandlerDispatcher_PUT, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandlerDispatcher rhd;

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPost([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Changed);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, rhd.PUT(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_POST, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandlerDispatcher rhd;

  // Now the URI exists and should be found
  rhd.onUri("/bar").onPut([](const Path& uri, const std::string& payload){
    return CoAP::RestResponse().withCode(CoAP::Code::Created);
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, rhd.POST(Path("/bar"), "Something").code());
}

TEST(ResponseHandlerDispatcher_DELETE, ReturnsMethodNotAllowedForKnownURIWithoutHandler) {
  CoAP::RequestHandlerDispatcher rhd;

  rhd.onUri("/bar").onGet([](const Path& uri){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload("something");
  });

  ASSERT_EQ(CoAP::Code::MethodNotAllowed, rhd.DELETE(Path("/bar")).code());
}
