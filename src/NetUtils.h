/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __NetUtils_h
#define __NetUtils_h

#include <arpa/inet.h>
#include <iosfwd>
#include <netdb.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>

namespace CoAP {

class NetUtils {
 public:
  in_addr_t ipFromHostname(const std::string& hostname);

 protected:
  // trampoline for unit test to override system call gethostbyname
  virtual hostent* gethostbyname(const std::string& server) const;
};

}  // namespace CoAP

#endif //__NetUtils_h
