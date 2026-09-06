//===-- Unittests for ctermid ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/stdio_macros.h"
#include "src/stdio/ctermid.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcCtermidTest, WritesIntoTheGivenBuffer) {
  char buffer[L_ctermid];
  for (size_t i = 0; i < sizeof(buffer); ++i)
    buffer[i] = 'x';
  char *result = LIBC_NAMESPACE::ctermid(buffer);
  ASSERT_TRUE(result == buffer);
  ASSERT_STREQ(result, "/dev/tty");
}

// Where the caller passes nothing, the name goes somewhere of the library's
// own, which a later call may overwrite.
TEST(LlvmLibcCtermidTest, WithoutABuffer) {
  char *result = LIBC_NAMESPACE::ctermid(nullptr);
  ASSERT_TRUE(result != nullptr);
  ASSERT_STREQ(result, "/dev/tty");
}

// The name fits in an array of the size the macro states.
TEST(LlvmLibcCtermidTest, NameFitsTheStatedSize) {
  char buffer[L_ctermid];
  char *result = LIBC_NAMESPACE::ctermid(buffer);
  size_t len = 0;
  while (result[len] != '\0')
    ++len;
  ASSERT_LT(len, static_cast<size_t>(L_ctermid));
}
