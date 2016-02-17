/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Code.h"

#include <ostream>

namespace CoAP {


std::string toString(Code code) {
  switch (code) {
#define COAP_CODE(V, N) case Code::N: return #N;
    COAP_CODES
#undef COAP_CODE
  }
  return "<undefined>";
}


std::ostream& operator<<(std::ostream& os, Code rhs) {
  const auto code = 100 * (static_cast<unsigned>(rhs) >> 5) +
              (static_cast<unsigned>(rhs) & 0x1F);
  os << code << "-" << toString(rhs);
  return os;
}

}  // namespace CoAP
