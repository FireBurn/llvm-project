//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for swprintf.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "hdr/types/wint_t.h"
#include "hdr/wchar_macros.h"
#include "src/wchar/swprintf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSwprintfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSwprintfTest, FormatsIntoTheBuffer) {
  wchar_t buf[16];
  int result = LIBC_NAMESPACE::swprintf(buf, 16, L"%d-%ls", 42, L"ab");
  ASSERT_EQ(result, 5);
  const wchar_t expected[] = L"42-ab";
  for (int i = 0; i <= 5; ++i)
    ASSERT_EQ(static_cast<int>(buf[i]), static_cast<int>(expected[i]));
}

TEST_F(LlvmLibcSwprintfTest, FailsWhenTheBufferIsTooSmall) {
  wchar_t buf[4];
  ASSERT_EQ(LIBC_NAMESPACE::swprintf(buf, 4, L"%s", "toolong"), -1);
}

#if WCHAR_MAX > 0xFFFF
TEST_F(LlvmLibcSwprintfTest, KeepsCharactersBeyondASCII) {
  wchar_t buf[16];
  // Each comes out as one wide character, not the bytes that encode it.
  int result = LIBC_NAMESPACE::swprintf(buf, 16, L"%ls|%lc", L"€ß",
                                        static_cast<wint_t>(L'\U00010348'));
  ASSERT_EQ(result, 4);
  EXPECT_EQ(buf[0], L'€');
  EXPECT_EQ(buf[1], L'ß');
  EXPECT_EQ(buf[2], L'|');
  EXPECT_EQ(buf[3], L'\U00010348');
  EXPECT_EQ(buf[4], L'\0');
}
#endif
