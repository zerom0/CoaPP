/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RequestHandler_h
#define __RequestHandler_h

#include "Notifications.h"
#include "Path.h"
#include "RestResponse.h"

#include <functional>
#include <memory>

namespace CoAP {

class RequestHandlers;

class RequestHandler {
 public:
  explicit RequestHandler(RequestHandlers& parent) : parent_(&parent) { }

  RequestHandler() = default;
  RequestHandler(const RequestHandler&) = default;
  RequestHandler& operator=(const RequestHandler&) = default;

  CoAP::RestResponse GET(const Path& uri) const { return get_(uri); }

  CoAP::RestResponse PUT(const Path& uri, const std::string& payload) const { return put_(uri, payload); }

  CoAP::RestResponse POST(const Path& uri, const std::string& payload) const { return post_(uri, payload); }

  CoAP::RestResponse DELETE(const Path& uri) const { return delete_(uri); }

  CoAP::RestResponse OBSERVE(const Path& uri, std::weak_ptr<Notifications> notifications) const { return observe_(uri, notifications); }

  bool isGetDelayed() const { return getIsDelayed_; }

  bool isPutDelayed() const { return putIsDelayed_; }

  bool isPostDelayed() const { return postIsDelayed_; }

  bool isDeleteDelayed() const { return deleteIsDelayed_; }

  bool isObserveDelayed() const { return observeIsDelayed_; }

  using GetFunction = std::function<CoAP::RestResponse(const Path&)>;
  using PutFunction = std::function<CoAP::RestResponse(const Path&, const std::string&)>;
  using PostFunction = std::function<CoAP::RestResponse(const Path&, const std::string&)>;
  using DeleteFunction = std::function<CoAP::RestResponse(const Path&)>;
  using ObserveFunction = std::function<CoAP::RestResponse(const Path&, std::weak_ptr<CoAP::Notifications>)>;

  RequestHandler& onGet(GetFunction func, bool delayed = false) {
    get_ = func;
    getIsDelayed_ = delayed;
    return *this;
  }

  RequestHandler& onPut(PutFunction func, bool delayed = false) {
    put_ = func;
    putIsDelayed_ = delayed;
    return *this;
  }

  RequestHandler& onPost(PostFunction func, bool delayed = false) {
    post_ = func;
    postIsDelayed_ = delayed;
    return *this;
  }

  RequestHandler& onDelete(DeleteFunction func, bool delayed = false) {
    delete_ = func;
    deleteIsDelayed_ = delayed;
    return *this;
  }

  RequestHandler& onObserve(ObserveFunction func,
                            bool delayed = false) {
    observe_ = func;
    observeIsDelayed_ = delayed;
    return *this;
  }

  RequestHandler& onUri(std::string uri);

 private:
  GetFunction get_ = [](const Path&){ return CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed); };
  PutFunction put_ = [](const Path&, const std::string&){ return CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed); };
  PostFunction post_ = [](const Path&, const std::string&){ return CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed); };
  DeleteFunction delete_ = [](const Path&){ return CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed); };
  ObserveFunction observe_ = [](const Path&, std::weak_ptr<CoAP::Notifications>){ return CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed); };

  bool getIsDelayed_{false};
  bool putIsDelayed_{false};
  bool postIsDelayed_{false};
  bool deleteIsDelayed_{false};
  bool observeIsDelayed_{false};

  RequestHandlers* parent_;
};

}  // namespace CoAP

#endif  // __FluentRequestHandlerLeaf_h
