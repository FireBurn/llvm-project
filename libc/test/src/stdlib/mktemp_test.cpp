//===-- Unittests for mktemp and mkdtemp ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/stdlib/mkdtemp.h"
#include "src/stdlib/mktemp.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/rmdir.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcMkTempTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

TEST_F(LlvmLibcMkTempTest, ReplacesTheTemplate) {
  char tmpl[] = "mktemp.test.XXXXXX";
  char *result = LIBC_NAMESPACE::mktemp(tmpl);
  ASSERT_ERRNO_SUCCESS();
  // The name is returned in place, with the X's gone.
  EXPECT_TRUE(result == tmpl);
  EXPECT_EQ(string_view(tmpl).size(), sizeof(tmpl) - 1);
  EXPECT_TRUE(string_view(tmpl).starts_with("mktemp.test."));
  EXPECT_FALSE(string_view(tmpl).ends_with("XXXXXX"));

  // The name it gave back is one which is free.
  struct stat buf;
  EXPECT_NE(LIBC_NAMESPACE::stat(tmpl, &buf), 0);
  libc_errno = 0;
}

TEST_F(LlvmLibcMkTempTest, TooFewXsIsAnError) {
  char tmpl[] = "mktemp.test.XXXXX";
  char *result = LIBC_NAMESPACE::mktemp(tmpl);
  ASSERT_ERRNO_EQ(EINVAL);
  // A failure leaves an empty string, so the template cannot be used as a
  // name by a caller which did not check.
  EXPECT_TRUE(result == tmpl);
  EXPECT_TRUE(string_view(tmpl).empty());
}

TEST_F(LlvmLibcMkTempTest, NoTemplateAtAll) {
  char tmpl[] = "mktemp.test";
  LIBC_NAMESPACE::mktemp(tmpl);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_TRUE(string_view(tmpl).empty());
}

TEST_F(LlvmLibcMkTempTest, MkdtempMakesADirectory) {
  char tmpl[] = "mkdtemp.test.XXXXXX";
  char *result = LIBC_NAMESPACE::mkdtemp(tmpl);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_TRUE(result == tmpl);

  struct stat buf;
  ASSERT_EQ(LIBC_NAMESPACE::stat(tmpl, &buf), 0);
  EXPECT_TRUE(S_ISDIR(buf.st_mode));
  // The directory belongs to the caller alone.
  EXPECT_EQ(buf.st_mode & 0777, mode_t(0700));

  ASSERT_EQ(LIBC_NAMESPACE::rmdir(tmpl), 0);
}

TEST_F(LlvmLibcMkTempTest, MkdtempTwiceGivesTwoDirectories) {
  char first[] = "mkdtemp.test.XXXXXX";
  char second[] = "mkdtemp.test.XXXXXX";
  ASSERT_TRUE(LIBC_NAMESPACE::mkdtemp(first) != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::mkdtemp(second) != nullptr);
  EXPECT_FALSE(string_view(first) == string_view(second));

  ASSERT_EQ(LIBC_NAMESPACE::rmdir(first), 0);
  ASSERT_EQ(LIBC_NAMESPACE::rmdir(second), 0);
}

TEST_F(LlvmLibcMkTempTest, MkdtempTooFewXs) {
  char tmpl[] = "mkdtemp.test.XX";
  EXPECT_TRUE(LIBC_NAMESPACE::mkdtemp(tmpl) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
}
