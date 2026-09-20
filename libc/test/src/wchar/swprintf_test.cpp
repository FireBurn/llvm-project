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
