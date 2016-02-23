/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Responses_h
#define __Responses_h

#include "RestResponse.h"
#include <functional>

namespace CoAP {

class ResponsesImpl;

class Responses {
 public:
  Responses() = delete;
  Responses(ResponsesImpl* p);
  Responses(const Responses&) = delete;
  Responses(Responses&& r);
  Responses& operator=(Responses&& r);
  ~Responses();

  bool empty() const;
  size_t size() const;
  void emplace_back(RestResponse&& response);
  RestResponse get();

  void onResponse(std::function<void(const RestResponse&)> callback);;

 private:
  ResponsesImpl* pImpl_{0};
};

}  // namespace CoAP

#endif //__Responses_h
