/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __RestResponse_h
#define __RestResponse_h

#include "Code.h"

#include <netinet/in.h>
#include <string>

namespace CoAP {

/*
 * Class: RestResponse
 *
 * Response from a REST request.
 */
class RestResponse {
 public:
  /*
   * Method: fromIp
   *
   * Returns:
   *    The IP address of the server from which the response was received.
   */
  in_addr_t fromIp() const { return fromIP_; }

  /*
   * Method: withSenderIP
   *
   * Sets the sender IP address on a copy of this response.
   *
   * Parameters:
   *    fromIP - Sender IP address
   *
   * Returns:
   *    A copy of the response with the sender IP address set.
   */
  RestResponse withSenderIP(in_addr_t fromIP) const {
    auto copy(*this);
    copy.fromIP_ = fromIP;
    return copy;
  }

  /*
   * Method: fromPort
   *
   * Returns:
   *    The UDP port of the server from which the response was received.
   */
  uint16_t fromPort() const { return fromPort_; }

  /*
   * Method: withSenderPort
   *
   * Sets the sender port on a copy of this response.
   *
   * Parameters:
   *    fromPort - Sender port
   *
   * Returns:
   *    A copy of the response with the sender port set.
   */
  RestResponse withSenderPort(uint16_t fromPort) const {
    auto copy(*this);
    copy.fromPort_ = fromPort;
    return copy;
  }

  /*
   * Method: code
   *
   * Returns:
   *    The response <Code>.
   */
  Code code() const { return code_; }

  /*
   * Method: withCode
   *
   * Sets the response code on a copy of this response.
   *
   * Parameters:
   *    code - Response code
   *
   * Returns:
   *    A copy of the response with the response <Code> set.
   */
  RestResponse withCode(Code code) const {
    auto copy(*this);
    copy.code_ = code;
    return copy;
  }

  /*
   * Method: payload
   *
   * Returns:
   *    The response payload.
   */
  std::string payload() const { return payload_; }

  /*
   * Method: withPayload
   *
   * Sets the payload on a copy of this response.
   *
   * Parameters:
   *    payload - Payload
   *
   * Returns:
   *    A copy of the response with the payload set.
   */
  // Learning: In this simple example call by value is faster than call by const ref
  RestResponse withPayload(std::string payload) const {
    auto copy(*this);
    copy.payload_ = payload;
    return copy;
  }

  /*
   * Method: withContentFormat
   *
   * Sets the content format on a copy of this response.
   *
   * Parameters:
   *    contentFormat - Content format
   *
   * Returns:
   *    A copy of the response with the content format set.
   */
  RestResponse withContentFormat(uint8_t contentFormat) const {
    auto copy(*this);
    copy.hasContentFormat_ = true;
    copy.contentFormat_ = contentFormat;
    return copy;
  }

  /*
   * Method: hasContentFormat
   *
   * Returns:
   *    true if the contentformat was set, otherwise false.
   */
  bool hasContentFormat() const {
    return hasContentFormat_;
  }

  /*
   * Method: contentFormat
   *
   * Returns:
   *    The the content format if it was set.
   *
   * See:
   *    <hasContentFormat>
   */
  uint8_t contentFormat() const {
    return contentFormat_;
  }
 private:
  Code code_ = Code::NotFound;
  std::string payload_;
  bool hasContentFormat_{false};
  uint8_t contentFormat_;
  in_addr_t fromIP_;
  uint16_t fromPort_;
};

inline std::ostream& operator<<(std::ostream& os, const RestResponse& r) {
  os << r.code();
  return os;
}

}  // namespace CoAP

#endif  // __RestResponse_h
