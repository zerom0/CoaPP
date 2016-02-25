/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Arguments_h
#define __Arguments_h


#include <Optional.h>
#include <string>
#include <URI.h>

class Arguments {
 public:
  static Optional<Arguments> fromArgv(int argc, const char** argv);

  const std::string& getRequest() const {
    return request_;
  }

  const URI& getUri() const {
    return uri_;
  }

  const std::string& getPayload() const {
    return payload_;
  }

  const bool isConfirmable() const {
    return confirmable_;
  }

 private:
  void setRequest(const std::string& request) { request_ = request; }
  void setUri(const std::string& uri) { uri_ = URI::fromString(uri); }
  void setPayload(const std::string& payload) { payload_ = payload; }
  void setNonConfirmable() { confirmable_ = false; }

  std::string request_;
  URI uri_;
  std::string payload_;
  bool confirmable_{true};
};


#endif //__Arguments_h
