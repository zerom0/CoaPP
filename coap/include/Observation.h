/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Observation_h
#define __Observation_h

#include "RestResponse.h"

#include <functional>

namespace CoAP {

/**
 * The Observation object reflects the latest received value of the observed resource.
 * It allows the registration of a callback for incoming notifications. The observation
 * will be cancelled on object destruction or explicitly when the method cancel() is
 * being called.
 */
class Observation {
 public:
  // Get the last response
  RestResponse get();

  // Set action on notification
  void onNotification(std::function<void(const RestResponse&)> action);

  // Cancel the observation
  void cancel();
};

}

#endif //__Observation_h
