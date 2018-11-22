/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Message.h"

#include <tuple>

using namespace CoAP;

TEST(Message, isMoveable) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto movedMsg = std::move(msg);
  EXPECT_EQ("/some/where", movedMsg.path());
}

TEST(Message, getType) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  EXPECT_EQ(Type::NonConfirmable, msg.type());
}

TEST(Message, getCode) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  EXPECT_EQ(Code::PUT, msg.code());
}

TEST(Message, getPath) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  EXPECT_EQ("/some/where", msg.path());
}

TEST(Message, getQueriesEmpty) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users");
  EXPECT_EQ(0U, msg.queries().size());
}

TEST(Message, getQueriesOne) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18");
  ASSERT_EQ(1U, msg.queries().size());
  EXPECT_EQ("min_age=18", msg.queries().front());
}

TEST(Message, getQueriesTwo) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18&max_age=39");
  ASSERT_EQ(2U, msg.queries().size());
  EXPECT_EQ("min_age=18", msg.queries()[0]);
  EXPECT_EQ("max_age=39", msg.queries()[1]);
}

TEST(Message, serializeOneQuery) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(1U, msg2.queries().size());
  EXPECT_EQ("min_age=18", msg2.queries().front());
}

TEST(Message, serializeOneLongQuery) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?is_this_a_long_query=yes_it_is");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(1U, msg2.queries().size());
  EXPECT_EQ("is_this_a_long_query=yes_it_is", msg2.queries().front());
}

TEST(Message, serializeTwoQueries) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18&max_age=39");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(2U, msg2.queries().size());
  EXPECT_EQ("min_age=18", msg2.queries()[0]);
  EXPECT_EQ("max_age=39", msg2.queries()[1]);
}

TEST(Message, tokenLength) {
  EXPECT_EQ(0U, Message::tokenLength(0));
  EXPECT_EQ(1U, Message::tokenLength(0x01));
  EXPECT_EQ(1U, Message::tokenLength(0xff));
  EXPECT_EQ(2U, Message::tokenLength(0x0100));
  EXPECT_EQ(2U, Message::tokenLength(0xffff));
  EXPECT_EQ(3U, Message::tokenLength(0x010000));
  EXPECT_EQ(3U, Message::tokenLength(0xffffff));
  EXPECT_EQ(4U, Message::tokenLength(0x01000000));
  EXPECT_EQ(4U, Message::tokenLength(0xffffffff));
  EXPECT_EQ(5U, Message::tokenLength(0x0100000000));
  EXPECT_EQ(5U, Message::tokenLength(0xffffffffff));
  EXPECT_EQ(6U, Message::tokenLength(0x010000000000));
  EXPECT_EQ(6U, Message::tokenLength(0xffffffffffff));
  EXPECT_EQ(7U, Message::tokenLength(0x01000000000000));
  EXPECT_EQ(7U, Message::tokenLength(0xffffffffffffff));
  EXPECT_EQ(8U, Message::tokenLength(0x0100000000000000));
  EXPECT_EQ(8U, Message::tokenLength(0xffffffffffffffff));
}

TEST(Message, getToken) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 4711, "/some/where");
  EXPECT_EQ(4711U, msg.token());
}

TEST(Message, invalidTokenLength) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto buffer = msg.asBuffer();
  buffer[0] |= 0x0f;
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);
}

TEST(Message, invalidPath) {
  auto msg = Message(CoAP::Type::NonConfirmable, 0, CoAP::Code::GET, 0, "/is/this/a/problem?yes=no");
  auto buffer = msg.asBuffer();
  // Manipulation of the option length to be bigger than the remaining message
  buffer[7] |= 0x0e;
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);
}

TEST(Message, tooShort) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto buffer = msg.asBuffer();

  // Bad length 3
  buffer.resize(3);
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);

  // Bad length 2
  buffer.resize(2);
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);

  // Bad length 1
  buffer.resize(1);
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);

  // Bad length 0
  buffer.resize(0);
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);
}

TEST(Message, badVersion) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto buffer = msg.asBuffer();

  // Bad version 0
  buffer[0] &= 0x3f;
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);

  // Bad version 2
  buffer[0] |= 0x80;
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);

  // Bad version 3
  buffer[0] |= 0x40;
  EXPECT_THROW(Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer)), std::exception);
}

TEST(Message, convertAndBack) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(msg.token(), back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithObserveValue) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/some/where");
  msg.withObserveValue(true);
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(msg.token(), back.token());
  EXPECT_EQ(msg.path(), back.path());
  EXPECT_EQ(msg.optionalObserveValue().value(), back.optionalObserveValue().value());
}

TEST(Message, convertAndBackWithLongUri) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/is_this_a_long_test/yes_it_is");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(msg.token(), back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithToken_min) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(0U, back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithToken_4711) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 4711, "/some/where");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(4711U, back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithToken_max) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0xffffffffffffffff, "/some/where");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(0xffffffffffffffff, back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithPayload) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 0, "/some/where", "true");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(msg.path(), back.path());
  EXPECT_EQ(msg.payload(), back.payload());
}

TEST(Message, convertAndBackWithContentFormat0Byte) {
  auto msg = Message(Type::NonConfirmable, 0, Code::Content, 0, "");
  EXPECT_FALSE(msg.optionalContentFormat());
  msg.withContentFormat(0);
  EXPECT_TRUE(msg.optionalContentFormat());
  EXPECT_EQ(0, msg.optionalContentFormat().value());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(5U, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_TRUE(msg2.optionalContentFormat());
  EXPECT_EQ(0, msg2.optionalContentFormat().value());
}

TEST(Message, convertAndBackWithContentFormat1Byte) {
  auto msg = Message(Type::NonConfirmable, 0, Code::Content, 0, "");
  EXPECT_FALSE(msg.optionalContentFormat());
  msg.withContentFormat(33);
  EXPECT_TRUE(msg.optionalContentFormat());
  EXPECT_EQ(33, msg.optionalContentFormat().value());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(6U, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_TRUE(msg2.optionalContentFormat());
  EXPECT_EQ(33, msg2.optionalContentFormat().value());
}

TEST(Message, convertAndBackWithContentFormat2Byte) {
  auto msg = Message(Type::NonConfirmable, 0, Code::Content, 0, "");
  EXPECT_FALSE(msg.optionalContentFormat());
  msg.withContentFormat(333);
  EXPECT_TRUE(msg.optionalContentFormat());
  EXPECT_EQ(333, msg.optionalContentFormat().value());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(7U, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_TRUE(msg2.optionalContentFormat());
  EXPECT_EQ(333, msg2.optionalContentFormat().value());
}

TEST(Message_option, baseAndOffset) {
  uint8_t buffer[] = {0, 0, 0};

  unsigned option{0};
  unsigned consumed_bytes{0};

  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(0U, option);
  EXPECT_EQ(1U, consumed_bytes);

  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(99), buffer);
  EXPECT_EQ(99U, option);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0x10;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(99), buffer);
  EXPECT_EQ(100U, option);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0xc0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(12U, option);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0xd0;
  buffer[1] = 0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(13U, option);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[1] = 0x1;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14U, option);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[1] = 0xff;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(268U, option);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[0] = 0xe0;
  buffer[1] = 0;
  buffer[2] = 0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269U, option);
  EXPECT_EQ(3U, consumed_bytes);

  buffer[1] = 0x01;
  buffer[2] = 0x00;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(525U, option);
  EXPECT_EQ(3U, consumed_bytes);

  buffer[1] = 0xff;
  buffer[2] = 0xff;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(65804U, option);
  EXPECT_EQ(3U, consumed_bytes);
}

TEST(Message_option, length) {
  uint8_t buffer[] = {0, 0, 0};

  unsigned length{0};
  unsigned consumed_bytes{0};

  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(0U, length);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0x01;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(1U, length);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0x0c;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(12U, length);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0x0d;
  buffer[1] = 0;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(13U, length);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[1] = 0x1;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14U, length);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[1] = 0xff;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(268U, length);
  EXPECT_EQ(2U, consumed_bytes);

  buffer[0] = 0x0e;
  buffer[1] = 0;
  buffer[2] = 0;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269U, length);
  EXPECT_EQ(3U, consumed_bytes);

  buffer[1] = 0x01;
  buffer[2] = 0x00;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(525U, length);
  EXPECT_EQ(3U, consumed_bytes);

  buffer[1] = 0xff;
  buffer[2] = 0xff;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(65804U, length);
  EXPECT_EQ(3U, consumed_bytes);
}

TEST(Message_option, baseOffsetAndLength) {
  uint8_t buffer[] = {0x12, 0, 0, 0, 0};

  unsigned option{0};
  unsigned length{0};
  unsigned consumed_bytes{0};

  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(1U, option);
  EXPECT_EQ(2U, length);
  EXPECT_EQ(1U, consumed_bytes);

  buffer[0] = 0xdd;
  buffer[1] = 0x01;
  buffer[2] = 0x02;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14U, option);
  EXPECT_EQ(15U, length);
  EXPECT_EQ(3U, consumed_bytes);

  buffer[0] = 0xed;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0x02;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269U, option);
  EXPECT_EQ(15U, length);
  EXPECT_EQ(4U, consumed_bytes);

  buffer[0] = 0xee;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = 1;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269U, option);
  EXPECT_EQ(270U, length);
  EXPECT_EQ(5U, consumed_bytes);
}

TEST(Message_option, makeShort) {
  auto optionBuffer = Message::makeOptionHeader(2, 3);
  ASSERT_EQ(1U, optionBuffer.size());
  EXPECT_EQ(0x23, optionBuffer[0]);
}
/**/
TEST(Message_option, makeTwoByteLength) {
  auto optionBuffer = Message::makeOptionHeader(2, 13);
  ASSERT_EQ(2U, optionBuffer.size());
  EXPECT_EQ(0x2d, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
}

TEST(Message_option, makeThreeByteLength) {
  auto optionBuffer = Message::makeOptionHeader(2, 269);
  ASSERT_EQ(3U, optionBuffer.size());
  EXPECT_EQ(0x2e, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
  EXPECT_EQ(0x0, optionBuffer[2]);
}

TEST(Message_option, testRangeOfLength) {
  for (unsigned inputLength = 0; inputLength < 269 + 256 * 256; ++inputLength) {
    auto optionBuffer = Message::makeOptionHeader(2, inputLength);
    unsigned resultLength = 0;
    std::tie(std::ignore, resultLength, std::ignore) =
        Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), optionBuffer.data());
    ASSERT_EQ(inputLength, resultLength);
  }
}

TEST(Message_option, makeTwoByteOption) {
  auto optionBuffer = Message::makeOptionHeader(13, 3);
  ASSERT_EQ(2U, optionBuffer.size());
  EXPECT_EQ(0xd3, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
}

TEST(Message_option, makeThreeByteOption) {
  auto optionBuffer = Message::makeOptionHeader(269, 3);
  ASSERT_EQ(3U, optionBuffer.size());
  EXPECT_EQ(0xe3, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
  EXPECT_EQ(0x0, optionBuffer[2]);
}

TEST(Message_option, testRangeOfOption) {
  for (unsigned inputOption = 0; inputOption < 269 + 256 * 256; ++inputOption) {
    auto optionBuffer = Message::makeOptionHeader(inputOption, 3);
    unsigned resultOption = 0;
    std::tie(resultOption, std::ignore, std::ignore) =
        Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), optionBuffer.data());
    ASSERT_EQ(inputOption, resultOption);
  }
}

TEST(Message_option, testRangeOfOptionAndLength) {
  std::vector<unsigned> inputLengths = {0, 1, 12, 13, 268, 269, 512, 1024, 10000, 256*256, 268 + 256 * 256};
  std::vector<unsigned> inputOptions = {0, 1, 12, 13, 268, 269, 512, 1024, 10000, 256*256, 268 + 256 * 256};
  for (unsigned inputLength : inputLengths) {
    for (unsigned inputOption : inputOptions) {
      auto optionBuffer = Message::makeOptionHeader(inputOption, inputLength);
      unsigned resultOption = 0;
      unsigned resultLength = 0;
      std::tie(resultOption, resultLength, std::ignore) =
          Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), optionBuffer.data());
      EXPECT_EQ(inputOption, resultOption);
      ASSERT_EQ(inputLength, resultLength);
    }
  }
}

