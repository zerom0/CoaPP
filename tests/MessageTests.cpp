/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Message.h"

using namespace CoAP;

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
  EXPECT_EQ(0, msg.queries().size());
}

TEST(Message, getQueriesOne) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18");
  ASSERT_EQ(1, msg.queries().size());
  EXPECT_EQ("min_age=18", msg.queries().front());
}

TEST(Message, getQueriesTwo) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18&max_age=39");
  ASSERT_EQ(2, msg.queries().size());
  EXPECT_EQ("min_age=18", msg.queries()[0]);
  EXPECT_EQ("max_age=39", msg.queries()[1]);
}

TEST(Message, serializeOneQuery) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(1, msg2.queries().size());
  EXPECT_EQ("min_age=18", msg2.queries().front());
}

TEST(Message, serializeOneLongQuery) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?is_this_a_long_query=yes_it_is");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(1, msg2.queries().size());
  EXPECT_EQ("is_this_a_long_query=yes_it_is", msg2.queries().front());
}

TEST(Message, serializeTwoQueries) {
  auto msg = Message(Type::NonConfirmable, 0, Code::GET, 0, "/users?min_age=18&max_age=39");
  auto msg2 = Message::fromBuffer(msg.asBuffer());
  ASSERT_EQ(2, msg2.queries().size());
  EXPECT_EQ("min_age=18", msg2.queries()[0]);
  EXPECT_EQ("max_age=39", msg2.queries()[1]);
}

TEST(Message, tokenLength) {
  EXPECT_EQ(0, Message::tokenLength(0));
  EXPECT_EQ(1, Message::tokenLength(0x01));
  EXPECT_EQ(1, Message::tokenLength(0xff));
  EXPECT_EQ(2, Message::tokenLength(0x0100));
  EXPECT_EQ(2, Message::tokenLength(0xffff));
  EXPECT_EQ(3, Message::tokenLength(0x010000));
  EXPECT_EQ(3, Message::tokenLength(0xffffff));
  EXPECT_EQ(4, Message::tokenLength(0x01000000));
  EXPECT_EQ(4, Message::tokenLength(0xffffffff));
  EXPECT_EQ(5, Message::tokenLength(0x0100000000));
  EXPECT_EQ(5, Message::tokenLength(0xffffffffff));
  EXPECT_EQ(6, Message::tokenLength(0x010000000000));
  EXPECT_EQ(6, Message::tokenLength(0xffffffffffff));
  EXPECT_EQ(7, Message::tokenLength(0x01000000000000));
  EXPECT_EQ(7, Message::tokenLength(0xffffffffffffff));
  EXPECT_EQ(8, Message::tokenLength(0x0100000000000000));
  EXPECT_EQ(8, Message::tokenLength(0xffffffffffffffff));
}

TEST(Message, getToken) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 4711, "/some/where");
  EXPECT_EQ(4711, msg.token());
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
  EXPECT_EQ(0, back.token());
  EXPECT_EQ(msg.path(), back.path());
}

TEST(Message, convertAndBackWithToken_4711) {
  auto msg = Message(Type::NonConfirmable, 0, Code::PUT, 4711, "/some/where");
  auto buffer = msg.asBuffer();
  auto back = Message::fromBuffer(static_cast<std::vector<uint8_t>>(buffer));
  EXPECT_EQ(msg.type(), back.type());
  EXPECT_EQ(msg.code(), back.code());
  EXPECT_EQ(4711, back.token());
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
  EXPECT_EQ(false, msg.hasContentFormat());
  msg.setContentFormat(0);
  EXPECT_EQ(true, msg.hasContentFormat());
  EXPECT_EQ(0, msg.contentFormat());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(5, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_EQ(true, msg2.hasContentFormat());
  EXPECT_EQ(0, msg2.contentFormat());
}

TEST(Message, convertAndBackWithContentFormat1Byte) {
  auto msg = Message(Type::NonConfirmable, 0, Code::Content, 0, "");
  EXPECT_EQ(false, msg.hasContentFormat());
  msg.setContentFormat(33);
  EXPECT_EQ(true, msg.hasContentFormat());
  EXPECT_EQ(33, msg.contentFormat());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(6, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_EQ(true, msg2.hasContentFormat());
  EXPECT_EQ(33, msg2.contentFormat());
}

TEST(Message, convertAndBackWithContentFormat2Byte) {
  auto msg = Message(Type::NonConfirmable, 0, Code::Content, 0, "");
  EXPECT_EQ(false, msg.hasContentFormat());
  msg.setContentFormat(333);
  EXPECT_EQ(true, msg.hasContentFormat());
  EXPECT_EQ(333, msg.contentFormat());

  auto buffer = msg.asBuffer();
  EXPECT_EQ(7, buffer.size());

  auto msg2 = Message::fromBuffer(buffer);
  EXPECT_EQ(true, msg2.hasContentFormat());
  EXPECT_EQ(333, msg2.contentFormat());
}

TEST(Message_option, baseAndOffset) {
  uint8_t buffer[] = {0, 0, 0};

  unsigned option{0};
  unsigned consumed_bytes{0};

  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(0, option);
  EXPECT_EQ(1, consumed_bytes);

  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(99), buffer);
  EXPECT_EQ(99, option);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0x10;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(99), buffer);
  EXPECT_EQ(100, option);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0xc0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(12, option);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0xd0;
  buffer[1] = 0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(13, option);
  EXPECT_EQ(2, consumed_bytes);

  buffer[1] = 0x1;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14, option);
  EXPECT_EQ(2, consumed_bytes);

  buffer[1] = 0xff;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(268, option);
  EXPECT_EQ(2, consumed_bytes);

  buffer[0] = 0xe0;
  buffer[1] = 0;
  buffer[2] = 0;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269, option);
  EXPECT_EQ(3, consumed_bytes);

  buffer[1] = 0x01;
  buffer[2] = 0x00;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(525, option);
  EXPECT_EQ(3, consumed_bytes);

  buffer[1] = 0xff;
  buffer[2] = 0xff;
  std::tie(option, std::ignore, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(65804, option);
  EXPECT_EQ(3, consumed_bytes);
}

TEST(Message_option, length) {
  uint8_t buffer[] = {0, 0, 0};

  unsigned length{0};
  unsigned consumed_bytes{0};

  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(0, length);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0x01;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(1, length);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0x0c;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(12, length);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0x0d;
  buffer[1] = 0;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(13, length);
  EXPECT_EQ(2, consumed_bytes);

  buffer[1] = 0x1;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14, length);
  EXPECT_EQ(2, consumed_bytes);

  buffer[1] = 0xff;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(268, length);
  EXPECT_EQ(2, consumed_bytes);

  buffer[0] = 0x0e;
  buffer[1] = 0;
  buffer[2] = 0;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269, length);
  EXPECT_EQ(3, consumed_bytes);

  buffer[1] = 0x01;
  buffer[2] = 0x00;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(525, length);
  EXPECT_EQ(3, consumed_bytes);

  buffer[1] = 0xff;
  buffer[2] = 0xff;
  std::tie(std::ignore, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(65804, length);
  EXPECT_EQ(3, consumed_bytes);
}

TEST(Message_option, baseOffsetAndLength) {
  uint8_t buffer[] = {0x12, 0, 0, 0, 0};

  unsigned option{0};
  unsigned length{0};
  unsigned consumed_bytes{0};

  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(1, option);
  EXPECT_EQ(2, length);
  EXPECT_EQ(1, consumed_bytes);

  buffer[0] = 0xdd;
  buffer[1] = 0x01;
  buffer[2] = 0x02;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(14, option);
  EXPECT_EQ(15, length);
  EXPECT_EQ(3, consumed_bytes);

  buffer[0] = 0xed;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0x02;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269, option);
  EXPECT_EQ(15, length);
  EXPECT_EQ(4, consumed_bytes);

  buffer[0] = 0xee;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = 1;
  std::tie(option, length, consumed_bytes) =
      Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), buffer);
  EXPECT_EQ(269, option);
  EXPECT_EQ(270, length);
  EXPECT_EQ(5, consumed_bytes);
}

TEST(Message_option, makeShort) {
  auto optionBuffer = Message::makeOptionHeader(2, 3);
  ASSERT_EQ(1, optionBuffer.size());
  EXPECT_EQ(0x23, optionBuffer[0]);
}
/**/
TEST(Message_option, makeTwoByteLength) {
  auto optionBuffer = Message::makeOptionHeader(2, 13);
  ASSERT_EQ(2, optionBuffer.size());
  EXPECT_EQ(0x2d, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
}

TEST(Message_option, makeThreeByteLength) {
  auto optionBuffer = Message::makeOptionHeader(2, 269);
  ASSERT_EQ(3, optionBuffer.size());
  EXPECT_EQ(0x2e, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
  EXPECT_EQ(0x0, optionBuffer[2]);
}

TEST(Message_option, testRangeOfLength) {
  for (int inputLength = 0; inputLength < 269 + 256 * 256; ++inputLength) {
    auto optionBuffer = Message::makeOptionHeader(2, inputLength);
    unsigned resultLength = 0;
    std::tie(std::ignore, resultLength, std::ignore) =
        Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), optionBuffer.data());
    ASSERT_EQ(inputLength, resultLength);
  }
}

TEST(Message_option, makeTwoByteOption) {
  auto optionBuffer = Message::makeOptionHeader(13, 3);
  ASSERT_EQ(2, optionBuffer.size());
  EXPECT_EQ(0xd3, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
}

TEST(Message_option, makeThreeByteOption) {
  auto optionBuffer = Message::makeOptionHeader(269, 3);
  ASSERT_EQ(3, optionBuffer.size());
  EXPECT_EQ(0xe3, optionBuffer[0]);
  EXPECT_EQ(0x0, optionBuffer[1]);
  EXPECT_EQ(0x0, optionBuffer[2]);
}

TEST(Message_option, testRangeOfOption) {
  for (int inputOption = 0; inputOption < 269 + 256 * 256; ++inputOption) {
    auto optionBuffer = Message::makeOptionHeader(inputOption, 3);
    unsigned resultOption = 0;
    std::tie(resultOption, std::ignore, std::ignore) =
        Message::parseOptionHeader(static_cast<CoAP::Message::Option>(0), optionBuffer.data());
    ASSERT_EQ(inputOption, resultOption);
  }
}

TEST(Message_option, testRangeOfOptionAndLength) {
  std::vector<int> inputLengths = {0, 1, 12, 13, 268, 269, 512, 1024, 10000, 256*256, 268 + 256 * 256};
  std::vector<int> inputOptions = {0, 1, 12, 13, 268, 269, 512, 1024, 10000, 256*256, 268 + 256 * 256};
  for (int inputLength : inputLengths) {
    for (int inputOption : inputOptions) {
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

