/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "StringHelpers.h"

TEST(SplitFirst, onEmptyString) {
  auto parts = splitFirst("", '?');
  EXPECT_EQ("", parts.first);
  EXPECT_EQ("", parts.second);
}

TEST(SplitFirst, onlyToken) {
  auto parts = splitFirst("?", '?');
  EXPECT_EQ("", parts.first);
  EXPECT_EQ("", parts.second);
}

TEST(SplitFirst, tokenNotFound) {
  auto parts = splitFirst("hello world", '?');
  EXPECT_EQ("hello world", parts.first);
  EXPECT_EQ("", parts.second);
}

TEST(SplitFirst, tokenAtStart) {
  auto parts = splitFirst("?hello world", '?');
  EXPECT_EQ("", parts.first);
  EXPECT_EQ("hello world", parts.second);
}

TEST(SplitFirst, tokenInside) {
  auto parts = splitFirst("hello?world", '?');
  EXPECT_EQ("hello", parts.first);
  EXPECT_EQ("world", parts.second);
}

TEST(SplitFirst, tokenAtEnd) {
  auto parts = splitFirst("hello world?", '?');
  EXPECT_EQ("hello world", parts.first);
  EXPECT_EQ("", parts.second);
}

TEST(SplitFirst, moreThanOneToken) {
  auto parts = splitFirst("hello?wor?ld", '?');
  EXPECT_EQ("hello", parts.first);
  EXPECT_EQ("wor?ld", parts.second);
}

TEST(SplitAll, onEmptyString) {
  auto parts = splitAll("", '?');
  EXPECT_EQ(true, parts.empty());
}

TEST(SplitAll, tokenNotFound) {
  auto parts = splitAll("hello world", '?');
  ASSERT_EQ(1u, parts.size());
  EXPECT_EQ("hello world", parts.front());
}

TEST(SplitAll, tokenInside) {
  auto parts = splitAll("hello?world", '?');
  ASSERT_EQ(2u, parts.size());

  auto correctResult = std::vector<std::string>{"hello", "world"};
  EXPECT_EQ(correctResult, parts);
}

TEST(SplitAll, twoTokensInside) {
  auto parts = splitAll("hello?sunny?world", '?');
  ASSERT_EQ(3u, parts.size());

  auto correctResult = std::vector<std::string>{"hello", "sunny", "world"};
  EXPECT_EQ(correctResult, parts);
}

TEST(SplitAll, tokenAtStart) {
  auto parts = splitAll("?hello world", '?');
  ASSERT_EQ(2u, parts.size());

  auto correctResult = std::vector<std::string>{"", "hello world"};
  EXPECT_EQ(correctResult, parts);
}

TEST(SplitAll, tokenAtEnd) {
  auto parts = splitAll("hello world?", '?');
  ASSERT_EQ(2u, parts.size());

  auto correctResult = std::vector<std::string>{"hello world", ""};
  EXPECT_EQ(correctResult, parts);
}

TEST(SplitAll, onlyOneToken) {
  auto parts = splitAll("?", '?');
  ASSERT_EQ(2u, parts.size());

  auto correctResult = std::vector<std::string>{"", ""};
  EXPECT_EQ(correctResult, parts);
}

TEST(SplitAll, onlyTwoTokens) {
  auto parts = splitAll("??", '?');
  ASSERT_EQ(3u, parts.size());

  auto correctResult = std::vector<std::string>{"", "", ""};
  EXPECT_EQ(correctResult, parts);
}
