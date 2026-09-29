//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fwprintf and vfwprintf
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fread.h"
#include "src/wchar/fwide.h"
#include "src/wchar/fwprintf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFwprintfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFwprintfTest, WritesTheTextInUTF8) {
  const auto FILENAME = libc_make_test_file_path("fwprintf_text.test");
  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(file == nullptr);

  // What comes back is the count of wide characters, not of bytes.
  ASSERT_EQ(LIBC_NAMESPACE::fwprintf(file, L"%d %ls\n", 42, L"¢€"), 6);
  EXPECT_GT(LIBC_NAMESPACE::fwide(file, 0), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);

  file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  constexpr unsigned char EXPECTED[] = {'4',  '2',  ' ',  0xc2, 0xa2,
                                        0xe2, 0x82, 0xac, '\n'};
  unsigned char buffer[16] = {0};
  ASSERT_EQ(LIBC_NAMESPACE::fread(buffer, 1, sizeof(buffer), file),
            sizeof(EXPECTED));
  for (size_t i = 0; i < sizeof(EXPECTED); ++i)
    EXPECT_EQ(buffer[i], EXPECTED[i]);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
}

TEST_F(LlvmLibcFwprintfTest, RefusesAByteStream) {
  const auto FILENAME = libc_make_test_file_path("fwprintf_bytes.test");
  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(file == nullptr);
  ASSERT_LT(LIBC_NAMESPACE::fwide(file, -1), 0);
  EXPECT_EQ(LIBC_NAMESPACE::fwprintf(file, L"%d", 1), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
}
