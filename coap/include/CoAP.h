/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __CoAP_h
#define __CoAP_h

#include "Client.h"
#include "RequestHandler.h"
#include "RequestHandlers.h"
#include "IMessaging.h"

namespace CoAP {

/*
 * Function: newMessaging
 *
 * This is the entry point into the CoAP client and server and instantiates a
 * CoAP messaging system.
 *
 * Parameters:
 *    port - UDP port on which the server is listening for requests.
 *
 * Returns:
 *    An instance of the messaging system.
 *
 * See:
 *    <IMessaging>
 */
std::unique_ptr<IMessaging> newMessaging(uint16_t port = 5683);

}  // namespace CoAP

#endif // __CoAP_h
