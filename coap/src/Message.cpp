/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Message.h"

#include "Logging.h"
#include "Optional.h"
#include "Path.h"
#include "StringHelpers.h"

#include <deque>
#include <iostream>
#include <tuple>

SETLOGLEVEL(LLWARNING)

namespace CoAP {

Message::Message(Type type, MessageId messageId, Code code, uint64_t token, std::string path, std::string payload)
  : type_(type)
  , messageId_(messageId)
  , token_(token)
  , code_(code)
  , payload_(std::move(payload))
{
  auto parts = splitFirst(path, '?');
  path_ = std::move(parts.first);
  queries_ = splitAll(parts.second, '&');
}

void appendUnsigned(Message::Buffer& buffer, uint64_t value, unsigned length) {
  while (length-- > 0) {
    uint8_t byte = (value >> (length * 8)) & 0xff;
    buffer.emplace_back(byte);
  }
}

Message::Buffer Message::asBuffer() const {
  Buffer buffer;
  buffer.reserve(256);

  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |Ver| T |  TKL  |      Code     |          Message ID           |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |   Token (if any, TKL bytes) ...
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |   Options (if any) ...
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |1 1 1 1 1 1 1 1| Payload (if any) ...
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  // Message header (4 Byte)
  auto token_length = tokenLength(token_);
  buffer.emplace_back(0x40 | (static_cast<uint8_t>(type_) << 4) | token_length);
  buffer.emplace_back(static_cast<uint8_t>(code_));
  buffer.emplace_back(static_cast<uint8_t>((messageId_ >> 8) & 0xff));  // message id high byte
  buffer.emplace_back(static_cast<uint8_t>(messageId_ & 0xff));         // message id low byte

  // Token (optional)
  appendUnsigned(buffer, token_, token_length);

  // Options (optional)
  int option = 0;

  // Option: Observe
  if (observeValue_) {
    const int option_offset = Observe - option;
    option += option_offset;

    unsigned int length = tokenLength(observeValue_.value());

    auto optionHeader = makeOptionHeader(option_offset, length);
    std::copy(begin(optionHeader), end(optionHeader), std::back_inserter(buffer));

    appendUnsigned(buffer, observeValue_.value(), length);
  }

  // Option: Uri-Path
  auto path = Path(path_);
  for (int i = 0; i < path.size(); ++i) {
    const int option_offset = UriPath - option;
    option += option_offset;

    auto part = path.getPart(i);
    auto optionHeader = makeOptionHeader(option_offset, part.length());
    std::copy(begin(optionHeader), end(optionHeader), std::back_inserter(buffer));
    std::copy(begin(part), end(part), std::back_inserter(buffer));
  }

  // Option: Content-Format
  if (contentFormat_) {
    const int option_offset = ContentFormat - option;
    option += option_offset;

    unsigned int length = tokenLength(contentFormat_.value());

    auto optionHeader = makeOptionHeader(option_offset, length);
    std::copy(begin(optionHeader), end(optionHeader), std::back_inserter(buffer));

    appendUnsigned(buffer, contentFormat_.value(), length);
  }

  // Option: Uri-Query
  for (auto query : queries_) {
    const int option_offset = UriQuery - option;
    option += option_offset;

    auto optionHeader = makeOptionHeader(option_offset, query.length());
    std::copy(begin(optionHeader), end(optionHeader), std::back_inserter(buffer));
    std::copy(begin(query), end(query), std::back_inserter(buffer));
  }

  // Payload (optional)
  if (payload_.size()) {
    // Payload marker (only if payload > 0 bytes)
    buffer.push_back(0xff);

    std::copy(begin(payload_), end(payload_), std::back_inserter(buffer));
  }

  return buffer;
}

namespace {

std::tuple<Type, int> parse1stByte(uint8_t byte) {
  int version = ((byte >> 6) & 0x3);
  // Accept only version 1
  if (version != 0x1) throw std::exception();

  Type type = static_cast<Type>((byte >> 4) & 0x3);

  int token_length = byte & 0x0f;
  // Token is 0 to 8 bytes long
  if (token_length > 8) throw std::exception();

  return std::make_tuple(type, token_length);
};

template<typename T>
T parseUnsigned(std::vector<uint8_t>::const_iterator& it, unsigned length) {
  T value = 0;
  while (length-- > 0) {
    value <<= 8;
    value += *it++;
  }
  return value;
}

}  // namespace

Message Message::fromBuffer(const std::vector<uint8_t>& buffer) {
  auto it = begin(buffer);
  auto endOfBuffer = end(buffer);

  // Valid messages have at least 4 bytes
  if (buffer.size() < 4) throw std::exception();

  Type type;
  int token_length;
  std::tie(type, token_length) = parse1stByte(*it++);

  // second byte - code
  auto code = static_cast<Code>(*it++);

  // third & forth bytes - message ID
  auto msgId = parseUnsigned<uint16_t>(it, 2);

  // read the token
  auto token = parseUnsigned<uint64_t>(it, token_length);

  // read options
  auto option = EmptyOption;
  unsigned length{0};
  unsigned consumed_bytes{0};
  Optional<uint16_t> contentFormat;
  Optional<uint16_t> observeValue;
  bool observe = false;
  Buffer path_buffer;
  std::string queries = "?";
  while (it < endOfBuffer && *it != 0xff) {
    std::tie(option, length, consumed_bytes) = parseOptionHeader(option, &*it);
    it += consumed_bytes;
    if (length > std::distance(it, buffer.end())) {
      WLOG << "Received option " << option << " with invalid length=" << length << " bytes.\n";
      throw std::exception();
    }
    switch (option) {
      case Observe:
        observeValue = parseUnsigned<uint32_t>(it, length);
        break;

      case UriPath:
        path_buffer.push_back(length);
        std::copy(it, it + length, std::back_inserter(path_buffer));
        it += length;
        break;

      case ContentFormat:
        contentFormat = parseUnsigned<uint16_t>(it, length);
        break;

      case UriQuery:
        queries += std::string(it, it + length) + '&';
        it += length;
        break;

      default:
        // TODO: Handle unrecognized options according to the standard
        WLOG << "Unrecognized option " << option << " with length=" << length << " bytes.\n";
        break;
    }
  }

  auto path = Path::fromBuffer(path_buffer).toString() + queries.substr(0, queries.length() - 1);

  // read payload
  std::string payload;
  if (it < endOfBuffer) {
    if (*it++ == 0xff) {
      std::copy(it, endOfBuffer, std::back_inserter(payload));
    }
  }

  auto msg = Message(type, msgId, code, token, path, payload);
  if (contentFormat) msg.withContentFormat(contentFormat.value());
  if (observeValue) msg.withObserveValue(observeValue.value());
  return msg;
}

std::tuple<Message::Option, unsigned, unsigned> Message::parseOptionHeader(Option option, const uint8_t* buffer) {
  unsigned off{0};
  unsigned offset = (buffer[0] >> 4) & 0x0f;
  switch (offset) {
    case 13:
      offset = 13 + buffer[1];
      off = 1;
      break;
    case 14:
      offset = 269 + (buffer[1] << 8) + buffer[2];
      off = 2;
      break;
    case 15:
      // Todo: Payload marker or message format error
      throw std::exception();
      break;
  }

  unsigned length = buffer[0] & 0x0f;
  switch (length) {
    case 13:
      length = 13 + buffer[off + 1];
      off += 1;
      break;
    case 14:
      length = 269 + (buffer[off + 1] << 8) + buffer[off + 2];
      off += 2;
      break;
    case 15:
      // Todo: Payload marker or message format error
      throw std::exception();
      break;
  }

  return std::make_tuple(static_cast<Option>(static_cast<unsigned>(option) + offset), length, off + 1);
}

Message::Buffer Message::makeOptionHeader(unsigned int optionOffset, unsigned length) {
  Buffer buffer;

  unsigned partOption = 0;
  unsigned partLength = 0;

  if (length < 13) {
    partLength = length;
  }
  else if (length < 269) {
    partLength = 13;
  }
  else {
    partLength = 14;
  }

  if (optionOffset < 13) {
    partOption = optionOffset;
  }
  else if (optionOffset < 269) {
    partOption = 13;
  }
  else {
    partOption = 14;
  }

  buffer.emplace_back(partOption << 4 | partLength);

  if (optionOffset >= 269) {
    auto remainingOption = optionOffset - 269;
    buffer.emplace_back((remainingOption >> 8) & 0xff);
    buffer.emplace_back(remainingOption & 0xff);
  }
  else if (optionOffset >= 13) {
    auto remainingOption = optionOffset - 13;
    buffer.emplace_back(remainingOption & 0xff);
  }

  if (length >= 269) {
    auto remainingLenght = length - 269;
    buffer.emplace_back((remainingLenght >> 8) & 0xff);
    buffer.emplace_back(remainingLenght & 0xff);
  }
  else if (length >= 13) {
    auto remainingLenght = length - 13;
    buffer.emplace_back(remainingLenght & 0xff);
  }

  return buffer;
}

size_t Message::tokenLength(uint64_t token) {
  auto i = 8;

  while (i > 0) {
    auto b = static_cast<uint8_t>((token >> (i - 1) * 8) & 0xff);
    if (b) break;
    --i;
  }

  return i;
}

std::ostream& operator<<(std::ostream& ost, const Message& rhs) {
  ost << "code=" << static_cast<int>(rhs.code())
      << " type=" << static_cast<int>(rhs.type())
      << " msgId=" << rhs.messageId();
  return ost;
}

}  // namespace CoAP
