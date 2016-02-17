/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __ResponsesImpl_h
#define __ResponsesImpl_h

#include "RestResponse.h"
#include "ReferenceCountable.h"

#include <deque>

namespace CoAP {

class ResponsesImpl : public ReferenceCountable {
 public:
  ResponsesImpl(std::function<void()> f) : ReferenceCountable(f) { }

  bool empty() const { return responses_.empty(); }
  size_t size() const { return responses_.size(); }
  void emplace_back(RestResponse&& response) { responses_.emplace_back(response); }
  RestResponse get() { auto r = responses_.front(); responses_.pop_front(); return r; }
  void onResponse(std::function<void(const RestResponse&)> callback) { callback_ = callback; };
  void callback() { if (callback_) callback_(responses_.front()); }

 private:
  std::deque<RestResponse> responses_;
  std::function<void(const RestResponse&)> callback_{nullptr};

};

}  // namespace CoAP

#endif //__ResponsesImpl_h
