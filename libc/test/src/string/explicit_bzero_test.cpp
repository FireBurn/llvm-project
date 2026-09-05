//===-- Unittests for explicit_bzero --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/explicit_bzero.h"
#include "test/UnitTest/Test.h"

// explicit_bzero clears memory the way bzero does. What separates it is that
// the compiler may not drop the write where nothing reads it back, which is
// why it is called over a buffer that held a secret.
TEST(LlvmLibcExplicitBzeroTest, ClearsTheWholeBuffer) {
  char buf[16];
  for (size_t i = 0; i < sizeof(buf); ++i)
    buf[i] = static_cast<char>(i + 1);

  LIBC_NAMESPACE::explicit_bzero(buf, sizeof(buf));
  for (size_t i = 0; i < sizeof(buf); ++i)
    ASSERT_EQ(buf[i], char(0));
}

TEST(LlvmLibcExplicitBzeroTest, ClearsOnlyWhatItWasGiven) {
  char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};

  LIBC_NAMESPACE::explicit_bzero(buf, 4);
  for (size_t i = 0; i < 4; ++i)
    ASSERT_EQ(buf[i], char(0));
  for (size_t i = 4; i < sizeof(buf); ++i)
    ASSERT_EQ(buf[i], static_cast<char>(i + 1));
}

TEST(LlvmLibcExplicitBzeroTest, ALengthOfZeroWritesNothing) {
  char buf[4] = {1, 2, 3, 4};
  LIBC_NAMESPACE::explicit_bzero(buf, 0);
  for (size_t i = 0; i < sizeof(buf); ++i)
    ASSERT_EQ(buf[i], static_cast<char>(i + 1));
}
