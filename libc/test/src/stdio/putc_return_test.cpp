//===-- Unittest for what putc and friends return -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/FILE.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fputc.h"
#include "src/stdio/putc.h"
#include "test/UnitTest/Test.h"

// C requires these to return the character written, taken as an unsigned
// char, not a success code. Callers routinely compare the result against the
// character they passed, so returning zero reads as a failed write.
TEST(LlvmLibcPutcReturnTest, ReturnsTheCharacterWritten) {
  constexpr char FILENAME[] = "testdata/putc_return.test";
  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(file == nullptr);

  EXPECT_EQ(LIBC_NAMESPACE::putc('!', file), int('!'));
  EXPECT_EQ(LIBC_NAMESPACE::fputc('A', file), int('A'));

  // A byte above 0x7f comes back as an unsigned char, never sign extended.
  EXPECT_EQ(LIBC_NAMESPACE::putc(0xff, file), 0xff);
  EXPECT_EQ(LIBC_NAMESPACE::fputc(0x80, file), 0x80);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
}
