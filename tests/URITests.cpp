/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "URI.h"

TEST(URI, ParseFullURI) {
  auto uri = URI::fromString("coap://localhost:5683/.well-known/core");
  EXPECT_EQ("coap", uri.getProtocol());
  EXPECT_EQ("localhost", uri.getServer());
  EXPECT_EQ(5683, uri.getPort());
  EXPECT_EQ("/.well-known/core", uri.getPath());
}

TEST(URI, ParseAnotherFullURI) {
  auto uri = URI::fromString("coaps://somewhere:4711/home");
  EXPECT_EQ("coaps", uri.getProtocol());
  EXPECT_EQ("somewhere", uri.getServer());
  EXPECT_EQ(4711, uri.getPort());
  EXPECT_EQ("/home", uri.getPath());
}

TEST(URI, ParseURIWithoutPort) {
  auto uri = URI::fromString("coap://localhost/.well-known/core");
  EXPECT_EQ("coap", uri.getProtocol());
  EXPECT_EQ("localhost", uri.getServer());
  EXPECT_EQ(5683, uri.getPort());
  EXPECT_EQ("/.well-known/core", uri.getPath());
}
