/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Optional_h
#define __Optional_h

#include <functional>
#include <stdexcept>

template<typename T>
class Optional {
 public:
  /**
   * Creation without value
   */
  Optional() = default;

  /**
   * Creation with value (copy)
   */
  explicit Optional(const T& value) : value_(value), valueSet_(true) {}

  /**
   * Creation with value (move)
   */
  explicit Optional(T&& value) : value_(std::forward<T>(value)), valueSet_(true) {}

  /**
   * Inplace creation of an optional value
   */
  template <typename ...Args> Optional(Args&& ...args) : value_(std::forward<Args>(args)...), valueSet_(true) {}

  /**
   * Sets the optional value
   */
  Optional& operator=(const T& value) {
    value_ = value;
    valueSet_ = true;
    return *this;
  }

  /**
   * Sets the optional value
   */
  Optional& operator=(T&& value) {
    value_ = std::forward<T>(value);
    valueSet_ = true;
    return *this;
  }

  /**
   * Returns whether the value has been set
   */
  operator bool() const { return valueSet_; }

  /**
   * Returns the value, if set.
   * If the value was not set an exception is thrown.
   */
  const T& value() const {
    if (valueSet_)
      return value_;
    else
      throw std::runtime_error("Optional value not set");
  }

  /**
   * Returns the value, if set. Otherwise it returns the default value from the parameter.
   */
  const T& valueOr(const T& v) const {
    if (valueSet_)
      return value_;
    else
      return v;
  }

 private:
  T value_{T()};
  bool valueSet_{false};
};

template<typename T, typename U>
Optional<U> lift(const Optional<T>& ot, std::function<U(const T&)> f) {
  return ot ? f(ot.value())
            : Optional<U>();
}

#endif //__Optional_h
