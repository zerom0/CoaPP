/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Connection.h"

namespace {

class ModifiedConnection : public CoAP::Connection {

 protected:
  int socket(int, int, int) const override {
    return 3;  // a valid socket index
  }

  int bind(int, const struct sockaddr*, socklen_t) const override {
    return 0;  // success
  }

  int setsockopt(int, int, int, const void*, socklen_t) const override {
    return 0;
  }

};

}  // namespace

TEST(Connection_Send, FailWhenConnectionIsNotOpen) {
  // GIVEN a connection that was not opened
  auto conn = ModifiedConnection();

  // WHEN we call the function send

  // THEN we shall get an exception
  EXPECT_THROW(conn.send(CoAP::Telegram(0, 0, std::vector<uint8_t>())), std::logic_error);
}

TEST(Connection_Get, FailWhenConnectionIsNotOpen) {
  // GIVEN a connection that was not opened
  auto conn = ModifiedConnection();

  // WHEN we call the function send

  // THEN we shall get an exception
  EXPECT_THROW(conn.get(std::chrono::milliseconds(100)), std::logic_error);
}
