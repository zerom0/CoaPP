/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Message_h
#define __Message_h

#include "Code.h"
#include "Optional.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace CoAP {

enum class Type {
  Confirmable = 0,
  NonConfirmable = 1,
  Acknowledgement = 2,
  Reset = 3,
};

using MessageId = uint16_t;

/// This class describes the CoAP message
class Message {
 public:
  enum Option {
    EmptyOption = 0,
    Observe = 6,
    UriPath = 11,
    ContentFormat = 12,
    UriQuery = 15,
  };

  using Buffer = std::vector<uint8_t>;

  Message() = default;

  /**
   * Generates a request message
   */
  Message(Type type, MessageId messageId, Code code, uint64_t token, std::string path, std::string payload = "");

  /// Accessor for the message type
  Type type() const { return type_; }

  /// Accessor for the message id
  MessageId messageId() const { return messageId_; }

  /// Accessor for the message code
  Code code() const { return code_; }

  /// Returns whether the message has a request code
  bool isRequestCode() const { return static_cast<int>(code_) < 0x40; }

  /// Accessor for the token
  uint64_t token() const { return token_; }

  /// Accessor for the uri path
  std::string path() const { return path_; }

  /// Accessor for the queries
  std::vector<std::string> queries() const { return queries_; }

  /// Accessor for content format
  bool hasContentFormat() const { return contentFormat_; }

  uint16_t contentFormat() const { return contentFormat_.value(); }

  Message& withContentFormat(uint16_t contentFormat) {
    contentFormat_ = contentFormat;
    return *this;
  }

  /// Accessor for observe value
  bool hasObserveValue() const { return observeValue_; }

  uint16_t observeValue() const { return observeValue_.value(); }

  Message& withObserveValue(uint16_t observeValue) {
    observeValue_ = observeValue;
    return *this;
  }

  /// Accessor for the payload
  std::string payload() const { return payload_; }

  /// Convert the message into a buffer that can be transmitted over the network
  Buffer asBuffer() const;

  /// Convert a buffer received from the network into a message
  static Message fromBuffer(const std::vector<uint8_t>& buffer);

  static std::tuple<Option, unsigned, unsigned> parseOptionHeader(Option option, const uint8_t* buffer);

  static Buffer makeOptionHeader(unsigned int optionOffset, unsigned length);

  static size_t tokenLength(uint64_t token);

 private:
  Type type_{Type::Reset};
  MessageId messageId_{0};
  uint64_t token_{0};
  Code code_{Code::Empty};
  std::string path_;
  std::string payload_;
  std::vector<std::string> queries_;
  Optional<uint16_t> contentFormat_;
  Optional<uint16_t> observeValue_;
};

std::ostream& operator<<(std::ostream& ost, const Message& rhs);

}  // namespace CoAP

#endif  // __Message_h
