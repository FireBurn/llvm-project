//===-- Unittests for strerror --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/StringUtil/platform_errors.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/string/strerror.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStrErrorTest, KnownErrors) {
  ASSERT_STREQ(LIBC_NAMESPACE::strerror(0), "Success");

  for (auto [i, msg] : LIBC_NAMESPACE::PLATFORM_ERRORS)
    EXPECT_STREQ(LIBC_NAMESPACE::strerror(static_cast<int>(i)), msg.begin());
}

TEST(LlvmLibcStrErrorTest, UnknownErrors) {
  ASSERT_STREQ(LIBC_NAMESPACE::strerror(-1), "Unknown error -1");
  ASSERT_STREQ(LIBC_NAMESPACE::strerror(134), "Unknown error 134");
  ASSERT_STREQ(LIBC_NAMESPACE::strerror(2147483647),
               "Unknown error 2147483647");
  ASSERT_STREQ(LIBC_NAMESPACE::strerror(-2147483648),
               "Unknown error -2147483648");
}

// As for a signal with no name of its own: the terminator is not part of the
// text, and perror writes the text out by its size.
TEST(LlvmLibcStrErrorTest, ABuiltDescriptionIsNotTerminatedTwice) {
  const int cases[] = {-1, 134, 2147483647};
  for (const int err : cases) {
    LIBC_NAMESPACE::cpp::string_view text =
        LIBC_NAMESPACE::get_error_string(err);
    ASSERT_GT(text.size(), size_t(0));
    for (size_t i = 0; i < text.size(); ++i)
      ASSERT_NE(text[i], '\0');
  }
}
