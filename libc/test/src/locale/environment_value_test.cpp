//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for environment_value.
///
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/locale/locale.h"
#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::internal::environment_value;

TEST(LlvmLibcEnvironmentValue, ReadsWhatSetenvStored) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_ENV_VALUE", "a=b", 1), 0);
  const char *value = environment_value("LLVM_LIBC_ENV_VALUE");
  ASSERT_TRUE(value != nullptr);
  EXPECT_TRUE(string_view(value) == "a=b");

  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_ENV_VALUE", "", 1), 0);
  value = environment_value("LLVM_LIBC_ENV_VALUE");
  ASSERT_TRUE(value != nullptr);
  EXPECT_TRUE(string_view(value).empty());
}

TEST(LlvmLibcEnvironmentValue, NamesMustMatchWhole) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_ENV_LONGER", "x", 1), 0);
  EXPECT_TRUE(environment_value("LLVM_LIBC_ENV") == nullptr);
  EXPECT_TRUE(environment_value("LLVM_LIBC_ENV_LONGER_STILL") == nullptr);
  EXPECT_TRUE(environment_value("LLVM_LIBC_ENV_LONGER") != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::unsetenv("LLVM_LIBC_ENV_LONGER"), 0);
}

TEST(LlvmLibcEnvironmentValue, UnsetVariableIsNull) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_ENV_GONE", "x", 1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::unsetenv("LLVM_LIBC_ENV_GONE"), 0);
  EXPECT_TRUE(environment_value("LLVM_LIBC_ENV_GONE") == nullptr);
}
