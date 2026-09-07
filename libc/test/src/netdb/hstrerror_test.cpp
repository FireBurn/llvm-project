//===-- Unittests for hstrerror -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/netdb_macros.h"
#include "src/netdb/hstrerror.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcHstrerrorTest, EachValueHasItsOwnWords) {
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(0), "Resolver Error 0 (no error)");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(HOST_NOT_FOUND), "Unknown host");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(TRY_AGAIN),
               "Host name lookup failure");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(NO_RECOVERY), "Unknown server error");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(NO_DATA),
               "No address associated with name");
}

// Anything else is still answered, rather than left as a null pointer for the
// caller to print.
TEST(LlvmLibcHstrerrorTest, AnythingElseIsStillAnswered) {
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(5), "Unknown resolver error");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(-1), "Unknown resolver error");
  ASSERT_STREQ(LIBC_NAMESPACE::hstrerror(9999), "Unknown resolver error");
}
