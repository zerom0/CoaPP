/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __ReferenceCountable_h
#define __ReferenceCountable_h

#include <functional>

class ReferenceCountable {
 public:
  virtual ~ReferenceCountable() = default;

  ReferenceCountable(std::function<void()> callback)
      : callback_(callback) {
  }

  void incrementCount() {
    ++count_;
  }

  void decrementCount() {
    --count_;
    if (count_ == 0) callback_();
  }

 private:
  int count_{0};
  std::function<void()> callback_;
};

#endif //__ReferenceCountable_h
