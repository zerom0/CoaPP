/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RequestHandlers_h
#define __RequestHandlers_h

#include "RequestHandler.h"
#include "PathPattern.h"
#include "RestResponse.h"

#include <vector>

namespace CoAP {

/**
 * Container for storing RequestHandlers and matching them to path patterns.
 */
class RequestHandlers {
 public:
  /**
   * Registers a new RequestHandler for the given path pattern and returns it for configuration like:
   *   requestHandlers.onUri("/abc").onGet(...)
   *
   * @param pathPattern Pattern for matching the RequestHandler.
   *
   * @return The RequestHandler for the specified path pattern.
   */
  RequestHandler& onUri(std::string pathPattern);

  /**
   * Returns the RequestHandler that matches the given path.
   *
   * @param path Path to find a RequestHandler for.
   *
   * @return Pointer to the matching RequestHandler or nullptr if no RequestHandler matches the path.
   */
  RequestHandler* getHandler(const Path &path);

 private:
  std::vector<std::pair<PathPattern, RequestHandler>> requestHandlers_;
};

}  // namespace CoAP

#endif  // __RequestHandlers_h
