/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "RequestHandlerDispatcher.h"

#include "RequestHandler.h"
#include "PathPattern.h"

namespace CoAP {

CoAP::RestResponse RequestHandlerDispatcher::GET(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.GET(uri);
  }

  return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
}

CoAP::RestResponse RequestHandlerDispatcher::PUT(const Path& uri, const std::string& payload) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.PUT(uri, payload);
  }

  return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
}

CoAP::RestResponse RequestHandlerDispatcher::POST(const Path& uri, const std::string& payload) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.POST(uri, payload);
  }

  return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
}

CoAP::RestResponse RequestHandlerDispatcher::DELETE(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.DELETE(uri);
  }

  return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
}

CoAP::RestResponse RequestHandlerDispatcher::OBSERVE(const Path &uri, std::weak_ptr<Observable<CoAP::RestResponse>> notifications) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.OBSERVE(uri, notifications);
  }

  return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
}

bool RequestHandlerDispatcher::isGetDelayed(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.isGetDelayed();
  }

  return true;
}

bool RequestHandlerDispatcher::isPutDelayed(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.isPutDelayed();
  }

  return true;
}

bool RequestHandlerDispatcher::isPostDelayed(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.isPostDelayed();
  }

  return true;
}

bool RequestHandlerDispatcher::isDeleteDelayed(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.isDeleteDelayed();
  }

  return true;
}

bool RequestHandlerDispatcher::isObserveDelayed(const Path& uri) {
  for (auto& handler : requestHandlers_) {
    auto match = handler.first.match(Path(uri));
    if (match) return handler.second.isObserveDelayed();
  }

  return true;
}

RequestHandler& RequestHandlerDispatcher::onUri(std::string pathPattern) {
  requestHandlers_.emplace_back(PathPattern(pathPattern), RequestHandler(*this));
  return requestHandlers_.back().second;
}

}  // namespace CoAP
