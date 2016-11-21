/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Optional.h"

TEST(Optional, InitiallyNotSet) {
  Optional<int> ov;
  EXPECT_FALSE(static_cast<bool>(ov));
}

TEST(Optional, TestWhenSet) {
  Optional<int> ov;
  ov = 15;
  EXPECT_TRUE(static_cast<bool>(ov));
}

TEST(Optional, GetValueWhenSet) {
  Optional<int> ov;
  ov = 23;
  EXPECT_EQ(23, ov.value());
}

TEST(Optional, ExceptionOnGetWhenNetSet) {
  Optional<int> ov;
  EXPECT_THROW(ov.value(), std::runtime_error);
}

TEST(Optional, CreationWithValue) {
  auto ov = Optional<int>(23);
  EXPECT_TRUE(static_cast<bool>(ov));
  EXPECT_EQ(23, ov.value());
}

TEST(Optional, GetValueOrDefault) {
  Optional<int> ov;
  EXPECT_EQ(47, ov.valueOr(47));
  ov = 23;
  EXPECT_EQ(23, ov.valueOr(47));
}

TEST(Optional, MoveConstructor) {
  struct M {
    M() = default;
    M(const M& m) { ++copied_; }
    M(M&& m) { ++moved_; }

    unsigned copied_{0};
    unsigned moved_{0};
  };

  Optional<M> om(M{});
  EXPECT_EQ(0u, om.value().copied_);
  EXPECT_EQ(1u, om.value().moved_);
}

TEST(Optional, MoveAssignment) {
  struct M {
    M() = default;
    M(const M& m) { ++copied_; }
    M(M&& m) { ++moved_; }
    M& operator=(const M& m) { ++copied_; return *this; }
    M& operator=(M&& m) { ++moved_; return *this; }

    unsigned copied_{0};
    unsigned moved_{0};
  };

  Optional<M> om;
  om = M{};
  EXPECT_EQ(0u, om.value().copied_);
  EXPECT_EQ(1u, om.value().moved_);
}