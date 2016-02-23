/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Responses.h"

#include "ResponsesImpl.h"

namespace CoAP {

Responses::Responses(ResponsesImpl* p)
: pImpl_(p) {
  pImpl_->incrementCount();
}

Responses::Responses(Responses&& r) {
  pImpl_ = r.pImpl_;
  r.pImpl_ = nullptr;
}

Responses& Responses::operator=(Responses&& r) {
  if (pImpl_) pImpl_->decrementCount();
  pImpl_ = r.pImpl_;
  r.pImpl_ = nullptr;

  return *this;
}

Responses::~Responses() {
  if (pImpl_) pImpl_->decrementCount();
}

bool Responses::empty() const {
  return pImpl_->empty();
}

size_t Responses::size() const {
  return pImpl_->size();
}

void Responses::emplace_back(RestResponse&& response) {
  pImpl_->emplace_back(std::move(response));
}

RestResponse Responses::get() {
  return pImpl_->get();
}

void Responses::onResponse(std::function<void(const RestResponse&)> callback) {
  pImpl_->onResponse(callback);
}

}  // namespace CoAP

