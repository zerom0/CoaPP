/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "RequestHandlerDispatcher.h"

#include "Optional.h"
#include "RequestHandler.h"
#include "PathPattern.h"

#include <algorithm>

namespace CoAP {

template <typename C>
auto firstMatch(C &container, const Path &path) {
  auto it = std::find_if(std::begin(container), std::end(container), [path](auto& e){ return e.first.match(path); });
  return (it != std::end(container)) ? Optional<typename C::value_type::second_type>(it->second) : Optional<typename C::value_type::second_type>();
};

CoAP::RestResponse RequestHandlerDispatcher::GET(const Path& uri) {
  auto f = [uri](const auto& e){ return e.GET(uri); };
  return lift<RequestHandler, RestResponse>(firstMatch(requestHandlers_, uri), f)
         .valueOr(CoAP::RestResponse().withCode(CoAP::Code::NotFound));
}

CoAP::RestResponse RequestHandlerDispatcher::PUT(const Path& uri, const std::string& payload) {
  auto f = [uri, payload](const auto& e){ return e.PUT(uri, payload); };
  return lift<RequestHandler, RestResponse>(firstMatch(requestHandlers_, uri), f)
         .valueOr(CoAP::RestResponse().withCode(CoAP::Code::NotFound));
}

CoAP::RestResponse RequestHandlerDispatcher::POST(const Path& uri, const std::string& payload) {
  auto f = [uri, payload](const auto& e){ return e.POST(uri, payload); };
  return lift<RequestHandler, RestResponse>(firstMatch(requestHandlers_, uri), f)
         .valueOr(CoAP::RestResponse().withCode(CoAP::Code::NotFound));
}

CoAP::RestResponse RequestHandlerDispatcher::DELETE(const Path& uri) {
  auto f = [uri](const auto& e){ return e.DELETE(uri); };
  return lift<RequestHandler, RestResponse>(firstMatch(requestHandlers_, uri), f)
         .valueOr(CoAP::RestResponse().withCode(CoAP::Code::NotFound));
}

CoAP::RestResponse RequestHandlerDispatcher::OBSERVE(const Path &uri, std::weak_ptr<Observable<CoAP::RestResponse>> notifications) {
  auto f = [uri, notifications](const auto& e){ return e.OBSERVE(uri, notifications); };
  return lift<RequestHandler, RestResponse>(firstMatch(requestHandlers_, uri), f)
         .valueOr(CoAP::RestResponse().withCode(CoAP::Code::NotFound));
}

bool RequestHandlerDispatcher::isGetDelayed(const Path& uri) {
  auto f = [](const auto& e){ return e.isGetDelayed(); };
  return lift<RequestHandler, bool>(firstMatch(requestHandlers_, uri), f)
         .valueOr(false);
}

bool RequestHandlerDispatcher::isPutDelayed(const Path& uri) {
  auto f = [](const auto& e){ return e.isPutDelayed(); };
  return lift<RequestHandler, bool>(firstMatch(requestHandlers_, uri), f)
         .valueOr(false);
}

bool RequestHandlerDispatcher::isPostDelayed(const Path& uri) {
  auto f = [](const auto& e){ return e.isPostDelayed(); };
  return lift<RequestHandler, bool>(firstMatch(requestHandlers_, uri), f)
         .valueOr(false);
}

bool RequestHandlerDispatcher::isDeleteDelayed(const Path& uri) {
  auto f = [](const auto& e){ return e.isDeleteDelayed(); };
  return lift<RequestHandler, bool>(firstMatch(requestHandlers_, uri), f)
         .valueOr(false);
}

bool RequestHandlerDispatcher::isObserveDelayed(const Path& uri) {
  auto f = [](const auto& e){ return e.isObserveDelayed(); };
  return lift<RequestHandler, bool>(firstMatch(requestHandlers_, uri), f)
         .valueOr(false);
}

RequestHandler& RequestHandlerDispatcher::onUri(std::string pathPattern) {
  requestHandlers_.emplace_back(PathPattern(pathPattern), RequestHandler(*this));
  return requestHandlers_.back().second;
}

}  // namespace CoAP
