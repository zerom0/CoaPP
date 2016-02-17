/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "CoAP.h"

#include "Messaging.h"

namespace CoAP {

std::unique_ptr<IMessaging> newMessaging(uint16_t port) {
  return std::unique_ptr<IMessaging>(new Messaging(port));
}

}  // namespace CoAP