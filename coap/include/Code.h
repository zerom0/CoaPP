/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __ResponseCode_h
#define __ResponseCode_h

#include <iosfwd>

namespace CoAP {

#define COAP_CODES \
  COAP_CODE(0x00, Empty) \
  COAP_CODE(0x01, GET) \
  COAP_CODE(0x02, POST) \
  COAP_CODE(0x03, PUT) \
  COAP_CODE(0x04, DELETE) \
  COAP_CODE(0x41, Created)                  /* 2.01 */ \
  COAP_CODE(0x42, Deleted)                  /* 2.02 */ \
  COAP_CODE(0x43, Valid)                    /* 2.03 */ \
  COAP_CODE(0x44, Changed)                  /* 2.04 */ \
  COAP_CODE(0x45, Content)                  /* 2.05 */ \
  COAP_CODE(0x80, BadRequest)               /* 4.00 */ \
  COAP_CODE(0x81, Unauthorized)             /* 4.01 */ \
  COAP_CODE(0x82, BadOption)                /* 4.02 */ \
  COAP_CODE(0x83, Forbidden)                /* 4.03 */ \
  COAP_CODE(0x84, NotFound)                 /* 4.04 */ \
  COAP_CODE(0x85, MethodNotAllowed)         /* 4.05 */ \
  COAP_CODE(0x86, NotAcceptable)            /* 4.06 */ \
  COAP_CODE(0x8c, PreconditionFailed)       /* 4.12 */ \
  COAP_CODE(0x8d, RequestEntityTooLarge)    /* 4.13 */ \
  COAP_CODE(0x8f, UnsupportedContentFormat) /* 4.15 */ \
  COAP_CODE(0xa0, InternalServerError)      /* 5.00 */ \
  COAP_CODE(0xa1, NotImplemented)           /* 5.01 */ \
  COAP_CODE(0xa2, BadGateway)               /* 5.02 */ \
  COAP_CODE(0xa3, ServiceUnavailable)       /* 5.03 */ \
  COAP_CODE(0xa4, GatewayTimeout)           /* 5.04 */ \
  COAP_CODE(0xa5, ProxyingNotSupported)     /* 5.05 */


/*
 * Enum: Code
 *
 * Enumeration of the request and response codes.
 *
 * Requests:
 * - GET
 * - PUT
 * - POST
 * - DELETE
 *
 * Responses:
 * - Created
 * - Deleted
 * - Valid
 * - Changed
 * - Content
 * - BadRequest
 * - Unauthorized
 * - BadOption
 * - Forbidden
 * - NotFound
 * - MethodNotAllowed
 * - NotAcceptable
 * - PreconditionFailed
 * - RequestEntityTooLarge
 * - UnsupportedContentFormat
 * - InternalServerError
 * - NotImplemented
 * - BadGateway
 * - ServiceUnavailable
 * - GatewayTimeout
 * - ProxyingNotSupported
 *
 */
enum class Code {
#define COAP_CODE(V, N) N = V,
  COAP_CODES
#undef COAP_CODE
};


std::ostream& operator<<(std::ostream& os, Code rhs);

}  // namespace CoAP

#endif  // __ResponseCode_h
