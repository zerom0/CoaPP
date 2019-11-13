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

/*
 * Class: Message
 *
 * Objects of this class represent CoAP messages and are de-/serializable
 * from/to byte arrays.
 */
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

  /*
   * Constructor
   */
  Message(Type type, MessageId messageId, Code code, uint64_t token, std::string path, std::string payload = "");

  /*
   * Method: type
   *
   * Returns:
   *   The message type.
   */
  Type type() const { return type_; }

  /*
   * Method: messageId
   *
   * Returns:
   *   The message id.
   */
  MessageId messageId() const { return messageId_; }

  /*
   * Method: code
   *
   * Return:
   *   The message code.
   */
  Code code() const { return code_; }

  /*
   * Method: isRequestCode
   *
   * Returns:
   *   True the message code is a request code.
   */
  bool isRequestCode() const { return static_cast<int>(code_) < 0x40; }

  /*
   * Method: token
   *
   * Returns:
   *   The message token.
   */
  uint64_t token() const { return token_; }

  /*
   * Method: path
   *
   * Returns:
   *   The URI of the request.
   */
  std::string path() const { return path_; }

  /*
   * Method: queries
   *
   * Returns:
   *   The query parameters of the request.
   */
  std::vector<std::string> queries() const { return queries_; }

  /*
   * Method: contentFormat
   *
   * Returns:
   *   The content format of the payload.
   */
  Optional<uint16_t> optionalContentFormat() const { return contentFormat_; }

  /*
   * Method: withContentFormat
   *
   * Defines the content format of the payload.
   */
  Message& withContentFormat(uint16_t contentFormat) {
    contentFormat_ = contentFormat;
    return *this;
  }

  /*
   * Method: observeValue
   *
   * Returns:
   *   The value of the observe option.
   */
  Optional<uint16_t> optionalObserveValue() const { return observeValue_; }

  /*
   * Method: withObserveValue
   *
   * Defines the value of the observe option.
   */
  Message& withObserveValue(uint16_t observeValue) {
    observeValue_ = observeValue;
    return *this;
  }

  /*
   * Method: payload
   *
   * Returns:
   *   The message payload.
   */
  std::string payload() const { return payload_; }

  /*
   * Method: asBuffer
   *
   * Returns:
   *   The serialized message as a byte array.
   */
  Buffer asBuffer() const;

  /*
   * Method: fromBuffer
   *
   * Returns:
   *   A CoAP message object deserialized from the byte array.
   */
  static Message fromBuffer(const std::vector<uint8_t>& buffer);

  /*
   * Method: parseOptionHeader
   *
   * Helper function that reads the option from the raw coap byte array.
   *
   * Parameters:
   *   option - Last read option type
   *   buffer - Pointer to the first byte of the option
   *
   * Returns:
   *   Tuple with
   *     - new option type,
   *     - length of the option value and
   *     - offset to the option value in the buffer
   */
  static std::tuple<Message::Option, unsigned int, unsigned int> parseOptionHeader(Option option,
                                                                                   const uint8_t *buffer,
                                                                                   const uint8_t *end);

  /*
   * Method: makeOptionHeader
   *
   * Helper function that encodes an option into a byte array.
   *
   * Parameters:
   *   optionOffset - Offset of the option type from the last option type
   *   length       - Length of the option payload
   *
   * Returns:
   *   Byte array with the encoded option
   */
  static Buffer makeOptionHeader(unsigned int optionOffset, unsigned length);

  /*
   * Method: tokenLength
   *
   * Helper function that calculates the length of the encoded token based on its value.
   *
   * Parameters:
   *   token - Token value
   *
   * Returns:
   *   Length of the encoded token value
   */
  static size_t tokenLength(uint64_t token);

 private:
  // Mandatory message parts
  Type type_{Type::Reset};
  MessageId messageId_{0};
  uint64_t token_{0};
  Code code_{Code::Empty};
  std::string path_;
  std::string payload_;

  // Optional message parts
  std::vector<std::string> queries_;
  Optional<uint16_t> contentFormat_;
  Optional<uint16_t> observeValue_;
};

std::ostream& operator<<(std::ostream& ost, const Message& rhs);

}  // namespace CoAP

#endif  // __Message_h
