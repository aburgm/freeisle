#pragma once

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
