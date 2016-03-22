/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef COAP_NOTIFICATIONS_H
#define COAP_NOTIFICATIONS_H

#include "Observable.h"

namespace CoAP {

class RestResponse;

using NotificationsImpl = Observable<RestResponse>;

/*
 * Class: Notification
 *
 * An observable that unregisters itself from its source when it gets destroyed.
 */
class Notifications {
 public:
  Notifications(NotificationsImpl* impl, std::function<void()> onDelete) : impl_(impl), onDelete_(onDelete) { }

  ~Notifications() { onDelete_(); }

  /*
   * Method: subscribe
   *
   * Clients can register an Observer for new values.
   *
   * Parameters:
   *    callback - A Callback function that is to be called for each new value.
   */
  void subscribe(NotificationsImpl::Callback callback) { impl_->subscribe(callback); }

 private:
  NotificationsImpl* impl_;
  std::function<void()> onDelete_;
};


}  // namespace CoAP

#endif  // COAP_NOTIFICATIONS_H
