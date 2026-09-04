//===-- Unittests for fgetpos and fsetpos ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/fpos_t.h"
#include "src/__support/libc_errno.h"
#include "src/stdio/fclose.h"
#include "src/stdio/feof.h"
#include "src/stdio/fgetpos.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fread.h"
#include "src/stdio/fsetpos.h"
#include "src/stdio/ftell.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFGetPosTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFGetPosTest, RemembersAndReturnsToAPosition) {
  constexpr const char *FILENAME = "fgetpos.test";
  constexpr const char *CONTENTS = "abcdefghij";

  ::FILE *f = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite(CONTENTS, 1, 10, f), size_t(10));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  f = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_TRUE(f != nullptr);

  char buf[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(buf, 1, 3, f), size_t(3));

  fpos_t pos;
  ASSERT_EQ(LIBC_NAMESPACE::fgetpos(f, &pos), 0);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_EQ(LIBC_NAMESPACE::ftell(f), 3L);

  // Read on, then go back to where the position was taken.
  ASSERT_EQ(LIBC_NAMESPACE::fread(buf, 1, 3, f), size_t(3));
  EXPECT_EQ(buf[0], 'd');

  ASSERT_EQ(LIBC_NAMESPACE::fsetpos(f, &pos), 0);
  EXPECT_EQ(LIBC_NAMESPACE::ftell(f), 3L);
  ASSERT_EQ(LIBC_NAMESPACE::fread(buf, 1, 3, f), size_t(3));
  EXPECT_EQ(buf[0], 'd');
  EXPECT_EQ(buf[2], 'f');

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFGetPosTest, ClearsEndOfFile) {
  constexpr const char *FILENAME = "fsetpos.test";
  ::FILE *f = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("xy", 1, 2, f), size_t(2));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  f = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_TRUE(f != nullptr);

  fpos_t start;
  ASSERT_EQ(LIBC_NAMESPACE::fgetpos(f, &start), 0);

  char buf[8] = {};
  LIBC_NAMESPACE::fread(buf, 1, 8, f);
  EXPECT_NE(LIBC_NAMESPACE::feof(f), 0);

  // Going back to a position leaves the stream readable again.
  ASSERT_EQ(LIBC_NAMESPACE::fsetpos(f, &start), 0);
  EXPECT_EQ(LIBC_NAMESPACE::feof(f), 0);
  EXPECT_EQ(LIBC_NAMESPACE::fread(buf, 1, 1, f), size_t(1));
  EXPECT_EQ(buf[0], 'x');

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFGetPosTest, NullArgumentsFail) {
  fpos_t pos;
  EXPECT_EQ(LIBC_NAMESPACE::fgetpos(nullptr, &pos), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::fsetpos(nullptr, &pos), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}
