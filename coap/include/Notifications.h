/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef COAP_NOTIFICATIONS_H
#define COAP_NOTIFICATIONS_H

#include "Observable.h"

namespace CoAP {

class RestResponse;

using Notifications = Observable<RestResponse>;

}  // namespace CoAP

#endif  // COAP_NOTIFICATIONS_H
