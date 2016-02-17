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
  virtual CoAP::RestResponse GET(const Path& uri);
  virtual CoAP::RestResponse PUT(const Path& uri, const std::string& payload);
  virtual CoAP::RestResponse POST(const Path& uri, const std::string& payload);
  virtual CoAP::RestResponse DELETE(const Path& uri);

  virtual bool isGetDelayed(const Path& uri);
  virtual bool isPutDelayed(const Path& uri);
  virtual bool isPostDelayed(const Path& uri);
  virtual bool isDeleteDelayed(const Path& uri);

  RequestHandler& onUri(std::string pathPattern);

 private:
  std::vector<std::pair<PathPattern, RequestHandler>> requestHandlers_;

};

}  // namespace CoAP

#endif  // __RequestHandlerDispatcher_h
