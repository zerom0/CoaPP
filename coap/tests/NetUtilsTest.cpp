/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "NetUtils.h"

#include <stdexcept>

namespace {

class NetUtilsMock: public CoAP::NetUtils {

 protected:
  hostent* gethostbyname(const std::string& server) const override {
    return nullptr;  // host could not be resolved
  }
};

}  // namespace

TEST(NetUtils, FailWhenServerCannotBeResolved) {
  EXPECT_THROW(NetUtilsMock().ipFromHostname("unavailable.local"), std::runtime_error);
}