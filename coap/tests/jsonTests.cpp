/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "json.h"

#include "gtest/gtest.h"

using namespace CoAP;

TEST(without, non_empty) {
  EXPECT_EQ("abc", __internal__::without('<', '>', "<abc>"));
}

TEST(without, empty) {
  EXPECT_EQ("", __internal__::without('<', '>', "<>"));
}

TEST(without, exception_without_opening_bracket) {
  EXPECT_THROW(__internal__::without('<', '>', "abc>"), std::runtime_error);
}

TEST(without, exception_without_closing_bracket) {
  EXPECT_THROW(__internal__::without('<', '>', "<abc"), std::runtime_error);
}

TEST(withoutCurly, non_empty) {
  EXPECT_EQ("abc", __internal__::withoutCurly("{abc}"));
}

TEST(withoutRect, non_empty) {
  EXPECT_EQ("abc", __internal__::withoutRect("[abc]"));
}

TEST(withoutQuot, non_empty) {
  EXPECT_EQ("abc", __internal__::withoutQuot("\"abc\""));
}

TEST(find_element_delimiter, simple) {
  EXPECT_EQ(1, __internal__::findListDelimiter("a,b"));
}

TEST(find_element_delimiter, nested) {
  EXPECT_EQ(5, __internal__::findListDelimiter("{a,b},c"));
}

TEST(trimmed, without_spaces) {
  EXPECT_EQ("hello", __internal__::trimmed("hello"));
}

TEST(trimmed, leading_spaces) {
  EXPECT_EQ("hello", __internal__::trimmed("   hello"));
}

TEST(trimmed, trailing_spaces) {
  EXPECT_EQ("hello", __internal__::trimmed("hello   "));
}

TEST(trimmed, one_space) {
  EXPECT_EQ("", __internal__::trimmed(" "));
}

TEST(trimmed, two_spaces) {
  EXPECT_EQ("", __internal__::trimmed(" "));
}

TEST(to_json, fromString) {
  EXPECT_EQ("\"text\"", to_json("text"));
}

TEST(from_json, toString) {
  std::string result;
  from_json(" \"text\" ", result);
  EXPECT_EQ("text", result);
}

TEST(to_json, fromStdString) {
  EXPECT_EQ("\"text\"", to_json(std::string("text")));
}

TEST(to_json, fromTrue) {
  EXPECT_EQ("true", to_json(true));
}

TEST(from_json, toTrue) {
  bool result;
  from_json("true", result);
  EXPECT_TRUE(result);
}

TEST(to_json, fromFalse) {
  EXPECT_EQ("false", to_json(false));
}

TEST(from_json, toFalse) {
  bool result;
  from_json("false", result);
  EXPECT_FALSE(result);
}

TEST(to_json, fromInt) {
  EXPECT_EQ("-123", to_json(-123));
}

TEST(from_json, toInt) {
  int result;
  from_json("-123", result);
  EXPECT_EQ(-123, result);
}

TEST(to_json, fromUInt) {
  EXPECT_EQ("123", to_json(123u));
}

TEST(from_json, toUInt) {
  unsigned int result;
  from_json("123", result);
  EXPECT_EQ(123, result);
}

TEST(to_json, fromDouble) {
  EXPECT_EQ("1.23", to_json(1.23));
}

TEST(to_json, fromNaturalDouble) {
  EXPECT_EQ("3", to_json(3));
}

TEST(to_json, fromNegativeDouble) {
  EXPECT_EQ("-3.141", to_json(-3.141));
}

TEST(from_json, toDouble) {
  double result;
  from_json("1.23", result);
  EXPECT_DOUBLE_EQ(1.23, result);
}

TEST(from_json, toNaturalDouble) {
  double result;
  from_json("3", result);
  EXPECT_DOUBLE_EQ(3, result);
}

TEST(from_json, toNegativeDouble) {
  double result;
  from_json("-3.141", result);
  EXPECT_DOUBLE_EQ(-3.141, result);
}

TEST(to_json, fromEmptyList) {
  EXPECT_EQ("[]", to_json(std::list<int>()));
}

TEST(from_json, toEmptyList) {
  std::list<int> result;
  from_json("[ ]", result);
  EXPECT_EQ(0, result.size());
}

TEST(to_json, fromListOfInts) {
  auto l = std::list<int>({1,2,3});
  EXPECT_EQ("[1,2,3]", to_json(l));
}

TEST(from_json, toListOfInts) {
  std::list<int> result;
  from_json("[1,2,3]", result);
  EXPECT_EQ(3, result.size());
  auto it = result.begin();
  EXPECT_EQ(1, *it++);
  EXPECT_EQ(2, *it++);
  EXPECT_EQ(3, *it++);
}

TEST(to_json, fromListOfStrings) {
  auto l = std::list<std::string>({"a", "b", "c"});
  EXPECT_EQ("[\"a\",\"b\",\"c\"]", to_json(l));
}

TEST(from_json, toListOfStrings) {
  std::list<std::string> result;
  from_json("[\"a\",\"b\",\"c\"]", result);
  EXPECT_EQ(3, result.size());
  auto it = result.begin();
  EXPECT_EQ("a", *it++);
  EXPECT_EQ("b", *it++);
  EXPECT_EQ("c", *it++);
}

TEST(to_json, fromMapOfStrings) {
  auto m = std::map<std::string, std::string>{{"Doe", "Joe"}, {"Dane", "Jane"}};
  EXPECT_EQ("{\"Dane\":\"Jane\",\"Doe\":\"Joe\"}", to_json(m));
}

TEST(from_json, toMapOfStrings) {
  std::map<std::string, std::string> m;
  from_json("{\"Doe\": \"Joe\", \"Dane\": \"Jane\"}", m);

  ASSERT_EQ(2, m.size());
  EXPECT_EQ("Jane", m["Dane"]);
  EXPECT_EQ("Joe", m["Doe"]);
}

TEST(to_json, fromMapOfMapOfInts) {
  auto gerade = std::map<std::string, int>{{"zwei", 2}};
  auto ungerade = std::map<std::string, int>{{"eins", 1}, {"drei", 3}};
  auto m = std::map<std::string, std::map<std::string, int>>{{"gerade", gerade}, {"ungerade", ungerade}};
  EXPECT_EQ("{\"gerade\":{\"zwei\":2},\"ungerade\":{\"drei\":3,\"eins\":1}}", to_json(m));
}

TEST(from_json, toMapOfMapOfInts) {
  auto m = std::map<std::string, std::map<std::string, int>>();
  from_json("{\"gerade\":{\"zwei\":2},\"ungerade\":{\"drei\":3,\"eins\":1}}", m);
  ASSERT_EQ(2, m.size());

  auto gerade = m["gerade"];
  ASSERT_EQ(1, gerade.size());
  EXPECT_EQ(2, gerade["zwei"]);

  auto ungerade = m["ungerade"];
  ASSERT_EQ(2, ungerade.size());
  EXPECT_EQ(1, ungerade["eins"]);
  EXPECT_EQ(3, ungerade["drei"]);
}
namespace {
class Object {
  int a_{0};
  int b_{0};
 public:
  Object() = default;
  Object(int a, int b) : a_(a), b_(b) { }

  bool operator==(const Object& rhs) const {
    return a_ == rhs.a_
        && b_ == rhs.b_;
  }

  std::string to_json() const {
    return ::to_json("a", a_) + "," + ::to_json("b", b_);
  }

  void from_json(const std::string& j) {
    ::from_json(j, "a", a_);
    ::from_json(j, "b", b_);
  }
};

};

TEST(to_json, fromObject) {
  EXPECT_EQ("{\"a\":1,\"b\":2}", to_json(Object(1,2)));
}

TEST(from_json, toObjectOriginalOrder) {
  Object result;
  from_json("{\"a\":1,\"b\":2}", result);
  EXPECT_EQ(Object(1, 2), result);
}

TEST(from_json, toObjectDifferentOrder) {
  Object result;
  from_json("{\"b\":2,\"a\":1}", result);
  EXPECT_EQ(Object(1, 2), result);
}

namespace {
class ObjectObject {
  Object obj_;
 public:
  ObjectObject() = default;
  ObjectObject(int a, int b) : obj_(a, b) { }

  bool operator==(const ObjectObject& rhs) const {
    return obj_ == rhs.obj_;
  }

  std::string to_json() const {
    return ::to_json("obj", obj_);
  }

  void from_json(const std::string& j) {
    ::from_json(j, "obj", obj_);
  }
};
}

TEST(to_json, fromObjectWithObject) {
  EXPECT_EQ("{\"obj\":{\"a\":1,\"b\":2}}", to_json(ObjectObject(1,2)));
}

TEST(from_json, toObjectWithObject) {
  ObjectObject result;
  from_json("{\"obj\":{\"a\":1,\"b\":2}}", result);
  EXPECT_EQ(ObjectObject(1,2), result);
}

namespace {
class ObjectWithLists {
  std::list<int> a_;
  std::list<int> b_;
 public:
  ObjectWithLists() = default;
  ObjectWithLists(std::list<int> a, std::list<int> b) : a_(a), b_(b) { }

  bool operator==(const ObjectWithLists& rhs) const {
    return a_ == rhs.a_
        && b_ == rhs.b_;
  }

  std::string to_json() const {
    return ::to_json("a", a_) + ","
        + ::to_json("b", b_);
  }

  void from_json(const std::string& j) {
    ::from_json(j, "a", a_);
    ::from_json(j, "b", b_);
  }
};
}

TEST(to_json, fromObjectWithList) {
  EXPECT_EQ("{\"a\":[1,2,3],\"b\":[4,5,6]}", to_json(ObjectWithLists({1, 2, 3}, {4, 5, 6})));
}

TEST(from_json, toObjectWithList) {
  ObjectWithLists result;
  from_json("{ \"a\" : [1, 2, 3], \"b\" : [4, 5, 6] }", result);
  EXPECT_EQ(ObjectWithLists({1, 2, 3}, {4, 5, 6}), result);
}

TEST(to_json, fromListWithObjects) {
  auto l = std::list<Object>({Object(1,2), Object(3,4)});
  EXPECT_EQ("[{\"a\":1,\"b\":2},{\"a\":3,\"b\":4}]", to_json(l));
}

TEST(from_json, toListWithObjects) {
  std::list<Object> result;
  from_json("[ { \"a\" : 1, \"b\" : 2 }, { \"a\" : 3, \"b\" : 4 } ]", result);
  EXPECT_EQ(std::list<Object>({Object(1,2), Object(3,4)}), result);
}