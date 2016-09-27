/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RequestHandler_h
#define __RequestHandler_h

#include "Observable.h"
#include "Path.h"
#include "RestResponse.h"

#include <functional>
#include <memory>

namespace CoAP {

class RequestHandlerDispatcher;

class RequestHandler {
 public:
  RequestHandler(RequestHandlerDispatcher& parent) : parent_(parent) { }

  virtual CoAP::RestResponse GET(const Path& uri) {
    return get_ ? get_(uri) : CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed);
  }

  virtual CoAP::RestResponse PUT(const Path& uri, const std::string& payload) {
    return put_ ? put_(uri, payload) : CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed);
  }

  virtual CoAP::RestResponse POST(const Path& uri, const std::string& payload) {
    return post_ ? post_(uri, payload) : CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed);
  }

  virtual CoAP::RestResponse DELETE(const Path& uri) {
    return delete_ ? delete_(uri) : CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed);
  }

  virtual CoAP::RestResponse OBSERVE(const Path& uri, std::weak_ptr<Observable<CoAP::RestResponse>> notifications) {
    return observe_ ? observe_(uri, notifications) : CoAP::RestResponse().withCode(CoAP::Code::MethodNotAllowed);
  }

  virtual bool isGetDelayed() {
    return getIsDelayed_;
  }

  virtual bool isPutDelayed() {
    return putIsDelayed_;
  }

  virtual bool isPostDelayed() {
    return postIsDelayed_;
  }

  virtual bool isDeleteDelayed() {
    return deleteIsDelayed_;
  }

  virtual bool isObserveDelayed() {
    return observeIsDelayed_;
  }

  using GetFunction = std::function<CoAP::RestResponse(const Path&)>;
  using PutFunction = std::function<CoAP::RestResponse(const Path&, const std::string& payload)>;
  using PostFunction = std::function<CoAP::RestResponse(const Path&, const std::string& payload)>;
  using DeleteFunction = std::function<CoAP::RestResponse(const Path&)>;
  using ObserveFunction = std::function<CoAP::RestResponse(const Path&, std::weak_ptr<Observable<CoAP::RestResponse>>)>;

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
  GetFunction get_;
  PutFunction put_;
  PostFunction post_;
  DeleteFunction delete_;
  ObserveFunction observe_;

  bool getIsDelayed_{false};
  bool putIsDelayed_{false};
  bool postIsDelayed_{false};
  bool deleteIsDelayed_{false};
  bool observeIsDelayed_{false};

  RequestHandlerDispatcher& parent_;
};

}  // namespace CoAP

#endif  // __FluentRequestHandlerLeaf_h
