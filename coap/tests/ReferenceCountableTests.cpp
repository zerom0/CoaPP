/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "ReferenceCountable.h"

class Derived : public ReferenceCountable {
 public:
  Derived(std::function<void()> f) : ReferenceCountable(f) { }
};

TEST(ReferenceCountable, NoCallbackCalledOnIncrement) {
  int called{0};
  ReferenceCountable rc([&called](){++called;});
  ASSERT_EQ(0, called);

  rc.incrementCount();
  EXPECT_EQ(0, called);
}

TEST(ReferenceCountable, CallbackCalledOnFinalDecrement) {
  int called{0};
  ReferenceCountable rc([&called](){++called;});
  ASSERT_EQ(0, called);

  rc.incrementCount();
  rc.incrementCount();
  ASSERT_EQ(0, called);

  rc.decrementCount();
  EXPECT_EQ(0, called);
  rc.decrementCount();
  EXPECT_EQ(1, called);
}

TEST(ReferenceCountable, CallbackCalledOnFinalDecrementDerived) {
  int called{0};
  Derived rc([&called](){++called;});
  ASSERT_EQ(0, called);

  rc.incrementCount();
  rc.incrementCount();
  ASSERT_EQ(0, called);

  rc.decrementCount();
  EXPECT_EQ(0, called);
  rc.decrementCount();
  EXPECT_EQ(1, called);
}
