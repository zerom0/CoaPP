/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Path.h"

TEST(Path, FromEmptyString) {
  auto p = Path("");
  EXPECT_EQ(0, p.size());
  EXPECT_THROW(p.getPart(0), std::range_error);
  EXPECT_EQ(0, p.toBuffer().size());
  EXPECT_EQ("", p.toString());
}

TEST(Path, OnlySlash) {
  auto p = Path("/");
  EXPECT_EQ(0, p.size());
  EXPECT_THROW(p.getPart(0), std::range_error);
  EXPECT_EQ(0, p.toBuffer().size());
  EXPECT_EQ("", p.toString());
}

TEST(Path, WithTwoParts) {
  auto p = Path("/home/root");
  EXPECT_EQ(2, p.size());
  EXPECT_EQ("home", p.getPart(0));
  EXPECT_EQ("root", p.getPart(1));
  EXPECT_THROW(p.getPart(2), std::range_error);
  EXPECT_EQ(10, p.toBuffer().size());
  EXPECT_EQ(4, p.toBuffer().at(0));
  EXPECT_EQ(4, p.toBuffer().at(5));
  EXPECT_EQ("/home/root", p.toString());
}

TEST(Path, WithTwoPartsWithTrailingSlash) {
  auto p = Path("/home/root/");
  EXPECT_EQ(2, p.size());
  EXPECT_EQ("home", p.getPart(0));
  EXPECT_EQ("root", p.getPart(1));
  EXPECT_THROW(p.getPart(2), std::range_error);
  EXPECT_EQ(10, p.toBuffer().size());
  EXPECT_EQ("/home/root", p.toString());
}

TEST(Path, WithLargeNumberOfParts) {
  auto p = Path("/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z");
  EXPECT_EQ(26, p.size());
  EXPECT_EQ("a", p.getPart(0));
  EXPECT_EQ("f", p.getPart(5));
  EXPECT_EQ("z", p.getPart(25));
}

