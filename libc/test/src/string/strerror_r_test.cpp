//===-- Unittests for strerror --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/string/strerror_r.h"
#include "test/UnitTest/Test.h"

#include "src/__support/macros/properties/os.h"

#include <stddef.h>

#ifdef LIBC_TARGET_OS_IS_ANDROID
// On Android strerror_r is the POSIX variant, which returns an error number.
TEST(LlvmLibcStrErrorRTest, PosixVariantTests) {
  char buffer[128] = {};
  ASSERT_EQ(LIBC_NAMESPACE::strerror_r(0, buffer, sizeof(buffer)), 0);
  ASSERT_STREQ(buffer, "Success");
  ASSERT_EQ(LIBC_NAMESPACE::strerror_r(-1, buffer, sizeof(buffer)), EINVAL);
  char small[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::strerror_r(0, small, sizeof(small)), ERANGE);
}
#else
// This tests the gnu variant of strerror_r (which returns a char*).
TEST(LlvmLibcStrErrorRTest, GnuVariantTests) {
  const size_t BUFF_SIZE = 128;
  char buffer[BUFF_SIZE];
  buffer[0] = '\0';
  // If strerror_r returns a constant string, then it shouldn't affect the
  // buffer.
  ASSERT_STREQ(LIBC_NAMESPACE::strerror_r(0, buffer, BUFF_SIZE), "Success");
  ASSERT_EQ(buffer[0], '\0');

  // Else it should write the result to the provided buffer.
  ASSERT_STREQ(LIBC_NAMESPACE::strerror_r(-1, buffer, BUFF_SIZE),
               "Unknown error -1");
  ASSERT_STREQ(buffer, "Unknown error -1");
}
#endif // LIBC_TARGET_OS_IS_ANDROID
