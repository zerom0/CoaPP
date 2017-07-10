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
auto firstMatch(C const & container, Path const & path) {
  using RV = typename C::value_type::second_type;
  auto it = std::find_if(std::begin(container), std::end(container), [path](auto & e){ return e.first.match(path); });
  return (it != std::end(container)) ? Optional<RV>(it->second) : Optional<RV>();
};

template <typename C>
auto callOnFirstMatch(C const & container, Path const & path, std::function<RestResponse(RequestHandler const &)> func) {
  return lift<RequestHandler, RestResponse>(func)(firstMatch(container, path))
         .valueOr(RestResponse().withCode(Code::NotFound));
}

template <typename C>
auto callOnFirstMatch(C const & container, Path const & path, std::function<bool(RequestHandler const &)> func) {
  return lift<RequestHandler, bool>(func)(firstMatch(container, path))
         .valueOr(false);
}

RestResponse RequestHandlerDispatcher::GET(const Path& uri) {
  return callOnFirstMatch(requestHandlers_, uri, [uri](const RequestHandler& e){ return e.GET(uri); });
}

RestResponse RequestHandlerDispatcher::PUT(const Path& uri, const std::string& payload) {
  return callOnFirstMatch(requestHandlers_, uri, [uri, payload](const RequestHandler& e){ return e.PUT(uri, payload); });
}

RestResponse RequestHandlerDispatcher::POST(const Path& uri, const std::string& payload) {
  return callOnFirstMatch(requestHandlers_, uri, [uri, payload](const RequestHandler& e){ return e.POST(uri, payload); });
}

RestResponse RequestHandlerDispatcher::DELETE(const Path& uri) {
  return callOnFirstMatch(requestHandlers_, uri, [uri](const RequestHandler& e){ return e.DELETE(uri); });
}

RestResponse RequestHandlerDispatcher::OBSERVE(const Path &uri, std::weak_ptr<Observable<RestResponse>> notifications) {
  return callOnFirstMatch(requestHandlers_, uri, [uri, notifications](const RequestHandler& e){ return e.OBSERVE(uri, notifications); });
}

bool RequestHandlerDispatcher::isGetDelayed(const Path& uri) const {
  return callOnFirstMatch(requestHandlers_, uri, [](const RequestHandler& e){ return e.isGetDelayed(); });
}

bool RequestHandlerDispatcher::isPutDelayed(const Path& uri) const {
  return callOnFirstMatch(requestHandlers_, uri, [](const RequestHandler& e){ return e.isPutDelayed(); });
}

bool RequestHandlerDispatcher::isPostDelayed(const Path& uri) const {
  return callOnFirstMatch(requestHandlers_, uri, [](const RequestHandler& e){ return e.isPostDelayed(); });
}

bool RequestHandlerDispatcher::isDeleteDelayed(const Path& uri) const {
  return callOnFirstMatch(requestHandlers_, uri, [](const RequestHandler& e){ return e.isDeleteDelayed(); });
}

bool RequestHandlerDispatcher::isObserveDelayed(const Path& uri) const {
  return callOnFirstMatch(requestHandlers_, uri, [](const RequestHandler& e){ return e.isObserveDelayed(); });
}

RequestHandler& RequestHandlerDispatcher::onUri(std::string pathPattern) {
  requestHandlers_.emplace_back(PathPattern(pathPattern), RequestHandler(*this));
  return requestHandlers_.back().second;
}

}  // namespace CoAP
