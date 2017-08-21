/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RequestHandlerDispatcher_h
#define __RequestHandlerDispatcher_h

#include "RequestHandler.h"
#include "PathPattern.h"
#include "RestResponse.h"

#include <vector>

namespace CoAP {

class RequestHandlerDispatcher {
 public:
  RequestHandler& onUri(std::string pathPattern);

  RequestHandler* getHandler(const Path &path);

 private:
  std::vector<std::pair<PathPattern, RequestHandler>> requestHandlers_;
};

}  // namespace CoAP

#endif  // __RequestHandlerDispatcher_h
