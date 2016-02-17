/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Parameters_h
#define __Parameters_h

#include <chrono>

namespace CoAP {

const auto ACK_TIMEOUT = std::chrono::milliseconds(1000);
const auto ACK_RANDOM_NUMBER = unsigned(150); // in Percent
const auto MAX_RETRANSMITS = unsigned(4);
const auto NSTART = unsigned(1);
const auto DEFAULT_LEASURE = double(5);
const auto PROBING_RATE = double(1);

}  // namespace CoAP

#endif //__Parameters_h
