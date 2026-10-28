//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// ile
/// Unittests for get_error_string.
///
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/span.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::span;
using LIBC_NAMESPACE::cpp::string_view;

TEST(LlvmLibcErrorToStringTest, BuiltDescriptionHasNoTerminatorInItsLength) {
  char buffer[64];
  for (char &c : buffer)
    c = 'x';

  string_view result =
      LIBC_NAMESPACE::get_error_string(12345, span<char>(buffer, 64));
  ASSERT_TRUE(result == "Unknown error 12345");
  // The data is still a C string for callers that hand it out as one.
  ASSERT_EQ(result.data()[result.size()], '\0');
}

TEST(LlvmLibcErrorToStringTest, SmallBufferGivesTheBaseDescription) {
  char buffer[4];
  string_view result =
      LIBC_NAMESPACE::get_error_string(12345, span<char>(buffer, 4));
  ASSERT_TRUE(result == "Unknown error");
}
