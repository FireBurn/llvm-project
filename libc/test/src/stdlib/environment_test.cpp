//===-- Unittests for clearenv and secure_getenv --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/clearenv.h"
#include "src/stdlib/getenv.h"
#include "src/stdlib/secure_getenv.h"
#include "src/stdlib/setenv.h"
#include "test/UnitTest/Test.h"

// A test binary is not run with raised privileges, so secure_getenv answers
// exactly as getenv does. What it hides is checked by the loader, which a
// unit test cannot arrange.
TEST(LlvmLibcEnvironmentTest, SecureGetenvReadsTheEnvironment) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_TEST_VAR", "value", 1), 0);

  const char *value = LIBC_NAMESPACE::secure_getenv("LLVM_LIBC_TEST_VAR");
  ASSERT_FALSE(value == nullptr);
  ASSERT_STREQ(value, "value");

  ASSERT_TRUE(LIBC_NAMESPACE::secure_getenv("LLVM_LIBC_ABSENT_VAR") == nullptr);
}

// clearenv leaves an environment with nothing in it, which is still an
// environment: a variable set after it is found again.
TEST(LlvmLibcEnvironmentTest, ClearenvEmptiesTheEnvironment) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_BEFORE_CLEAR", "x", 1), 0);
  ASSERT_FALSE(LIBC_NAMESPACE::getenv("LLVM_LIBC_BEFORE_CLEAR") == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::clearenv(), 0);
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_BEFORE_CLEAR") == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_AFTER_CLEAR", "y", 1), 0);
  const char *value = LIBC_NAMESPACE::getenv("LLVM_LIBC_AFTER_CLEAR");
  ASSERT_FALSE(value == nullptr);
  ASSERT_STREQ(value, "y");
}
