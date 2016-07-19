/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "PathPattern.h"

TEST(PathPattern, CompareEqualPaths) {
  auto a = PathPattern("/abba/cadabra");
  auto b = Path("/abba/cadabra");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathsWithTrailingSlash) {
  auto a = PathPattern("/abba/cadabra/");
  auto b = Path("/abba/cadabra");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareDifferentLength) {
  auto a = PathPattern("/abba/cadabra/gattaca");
  auto b = Path("/abba/cadabra");
  EXPECT_FALSE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithSingleWildcard) {
  auto a = PathPattern("/abba/?/gattaca");
  auto b = Path("/abba/cadabra/gattaca");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithMultipleWildcards) {
  auto a = PathPattern("/?/cadabra/?");
  auto b = Path("/abba/cadabra/gattaca");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithGreedyWildcard) {
  auto a = PathPattern("/abba/*");
  auto b = Path("/abba/cadabra/gattaca");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithGreedyWildcardAndSamePartCount) {
  auto a = PathPattern("/abba/*");
  auto b = Path("/abba/cadabra");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithGreedyWildcardCatchAll) {
  auto a = PathPattern("/*");
  auto b = Path("/abba/cadabra/gattaca");
  EXPECT_TRUE(a.match(b));
}

TEST(PathPattern, CompareEqualPathWithMixedWildcards) {
  auto a = PathPattern("/?/cadabra/*");
  auto b = Path("/abba/cadabra/gattaca/c");
  auto c = Path("/abba/cordoba/gattaca/c");
  EXPECT_TRUE(a.match(b));
  EXPECT_FALSE(a.match(c));
}