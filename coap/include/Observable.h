/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef COAP_OBSERVABLE_H
#define COAP_OBSERVABLE_H

#include <functional>

/*
 * Class: Observable
 *
 * An Observable represents a set of future values that are going to be received.
 */
template<class T> class Observable {
 public:
  using Callback = std::function<void(const T&)>;

  /*
   * Method: onNext
   *
   * onNext is being called when a new value is available.
   *
   * Parameters:
   *    value - The new value
   */
  void onNext(const T& value) const { if (callback_) callback_(value); }

  /*
   * Method: onClose
   *
   * onClose is being called when no further values will be available.
   */
  void onClose();

  /*
   * Method: onError
   *
   * onError is being called when the Observable has a problem. For example
   * when the Observable could not be instantiated.
   */
  void onError();

  /*
   * Method: subscribe
   *
   * Clients can register an Observer for new values.
   *
   * Parameters:
   *    callback - A Callback function that is to be called for each new value.
   */
  void subscribe(Callback callback) { callback_ = callback; }

 private:
  Callback callback_{nullptr};
};

#endif  // COAP_OBSERVABLE_H
