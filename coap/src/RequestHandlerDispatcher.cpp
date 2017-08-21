/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "RequestHandlerDispatcher.h"

#include "Optional.h"
#include "RequestHandler.h"
#include "PathPattern.h"

#include <algorithm>

namespace CoAP {

RequestHandler& RequestHandlerDispatcher::onUri(std::string pathPattern) {
  requestHandlers_.emplace_back(PathPattern(pathPattern), RequestHandler(*this));
  return requestHandlers_.back().second;
}

RequestHandler* RequestHandlerDispatcher::getHandler(const Path &path) {
  auto it = std::find_if(std::begin(requestHandlers_), std::end(requestHandlers_), [path](auto & e){ return e.first.match(path); });
  return (it != std::end(requestHandlers_)) ? &it->second : nullptr;
}

}  // namespace CoAP
