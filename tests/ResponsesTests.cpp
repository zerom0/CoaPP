/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Responses.h"
#include "ResponsesImpl.h"

TEST(Responses, ResponsesDestroysResponsesImpl) {
  int called{0};
  CoAP::ResponsesImpl ri([&called](){++called;});
  ASSERT_EQ(0, called);

  {
    CoAP::Responses r(&ri);
    EXPECT_EQ(0, called);
  }

  EXPECT_EQ(1, called);
}
