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
  CoAP::RestResponse GET(const Path& uri);
  CoAP::RestResponse PUT(const Path& uri, const std::string& payload);
  CoAP::RestResponse POST(const Path& uri, const std::string& payload);
  CoAP::RestResponse DELETE(const Path& uri);
  CoAP::RestResponse OBSERVE(const Path &uri, std::weak_ptr<Observable<CoAP::RestResponse>> notifications);

  bool isGetDelayed(const Path& uri) const;
  bool isPutDelayed(const Path& uri) const;
  bool isPostDelayed(const Path& uri) const;
  bool isDeleteDelayed(const Path& uri) const;
  bool isObserveDelayed(const Path& uri) const;

  RequestHandler& onUri(std::string pathPattern);

 private:
  std::vector<std::pair<PathPattern, RequestHandler>> requestHandlers_;

};

}  // namespace CoAP

#endif  // __RequestHandlerDispatcher_h
