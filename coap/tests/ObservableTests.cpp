/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Observable.h"

TEST(Observable, onNext_withoutObserver) {
  Observable<int> o;

  ASSERT_NO_THROW(o.onNext(23));
}

TEST(Observable, onNext_callsObserver) {
  Observable<int> o;

  int value = 0;

  o.subscribe([&value](const int& v){value = v;});
  o.onNext(23);

  ASSERT_EQ(23, value);
}