/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Code.h"

#include <sstream>

#include "gtest/gtest.h"

TEST(Code, TransformsIntoCorrectString) {
  std::stringstream ss;
  ss << CoAP::Code::Content;
  EXPECT_EQ("205-Content", ss.str());
}
