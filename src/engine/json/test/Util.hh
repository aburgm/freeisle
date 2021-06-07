#pragma once

#include <json/json.h>

#include <fmt/format.h>

#include <gtest/gtest.h>

// TODO(armin): this is available in gtest post 1.10:
#define ASSERT_THROW_KEEP_AS_E(statement, expected_exception)                  \
  std::exception_ptr _exceptionPtr;                                            \
  try {                                                                        \
    (statement);                                                               \
    FAIL() << "Expected: " #statement                                          \
              " throws an exception of type " #expected_exception              \
              ".\n  Actual: it throws nothing.";                               \
  } catch (expected_exception const &) {                                       \
    _exceptionPtr = std::current_exception();                                  \
  } catch (...) {                                                              \
    FAIL() << "Expected: " #statement                                          \
              " throws an exception of type " #expected_exception              \
              ".\n  Actual: it throws a different type.";                      \
  }                                                                            \
  try {                                                                        \
    std::rethrow_exception(_exceptionPtr);                                     \
  } catch (expected_exception const &e)

namespace freeisle::json::test {

inline void assert_json_equal(const Json::Value &v1, const Json::Value &v2) {
  ASSERT_TRUE(v2.isConvertibleTo(v1.type()));
  switch (v1.type()) {
  case Json::ValueType::nullValue:
    break;
  case Json::ValueType::intValue:
    EXPECT_EQ(v1.asInt64(), v2.asInt64());
    break;
  case Json::ValueType::uintValue:
    EXPECT_EQ(v1.asUInt64(), v2.asUInt64());
    break;
  case Json::ValueType::realValue:
    EXPECT_EQ(v1.asDouble(), v2.asDouble());
    break;
  case Json::ValueType::stringValue:
    EXPECT_EQ(v1.asString(), v2.asString());
    break;
  case Json::ValueType::booleanValue:
    EXPECT_EQ(v1.asBool(), v2.asBool());
    break;
  case Json::ValueType::arrayValue:
    ASSERT_EQ(v1.size(), v2.size());
    for (uint32_t i = 0; i < v1.size(); ++i) {
      SCOPED_TRACE(fmt::format("[{}]", i));
      assert_json_equal(v1[i], v2[i]);
    }
    break;
  case Json::ValueType::objectValue: {
    const Json::Value::Members m1 = v1.getMemberNames();
    const Json::Value::Members m2 = v2.getMemberNames();
    const std::set<std::string> ms1(m1.begin(), m1.end());
    const std::set<std::string> ms2(m2.begin(), m2.end());

    for (const std::string &s1 : ms1) {
      SCOPED_TRACE(fmt::format(".{}", s1));
      ASSERT_EQ(ms2.count(s1), 1);
    }

    for (const std::string &s2 : ms2) {
      SCOPED_TRACE(fmt::format(".{}", s2));
      ASSERT_EQ(ms1.count(s2), 1);
    }

    // should be ensured already by the two loops above:
    ASSERT_EQ(v1.size(), v2.size());

    for (const std::string &s : ms2) {
      SCOPED_TRACE(fmt::format(".{}", s));
      assert_json_equal(v1[s], v2[s]);
    }
  } break;
  default:
    FAIL() << "Unexpected JSON value type: " << v1.type();
    break;
  }
}

inline void check(const Json::Value &root, const std::string &expected) {
  Json::Reader reader;
  Json::Value expected_root;
  ASSERT_TRUE(reader.parse(expected, expected_root, false))
      << reader.getFormattedErrorMessages();

  SCOPED_TRACE("Trace needs to be read bottom-up");
  assert_json_equal(root, expected_root);
}

inline void check(const std::vector<uint8_t> result,
                  const std::string &expected) {
  Json::Reader reader;
  const char *begin = reinterpret_cast<const char *>(&result.data()[0]);
  const char *end = begin + result.size();

  Json::Value root;
  ASSERT_TRUE(reader.parse(begin, end, root, false))
      << reader.getFormattedErrorMessages();

  return check(root, expected);
}

inline void check(const std::vector<uint8_t> result,
                  const std::vector<uint8_t> &expected) {
  return check(result, std::string(expected.begin(), expected.end()));
}

} // namespace freeisle::json::test
