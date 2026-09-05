//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests for mkstemps and mkostemps
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/unistd_macros.h"
#include "src/stdlib/mkostemps.h"
#include "src/stdlib/mkstemps.h"
#include "src/string/strdup.h"
#include "src/string/strlen.h"
#include "src/unistd/access.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcMkstempsTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcMkstempsTest, SuffixIsKept) {
  char *tmpl =
      LIBC_NAMESPACE::strdup(libc_make_test_file_path("tmp_XXXXXX.txt"));
  const size_t len = LIBC_NAMESPACE::strlen(tmpl);
  int fd = LIBC_NAMESPACE::mkstemps(tmpl, 4);
  ASSERT_GE(fd, 0);
  EXPECT_EQ(tmpl[len - 4], '.');
  EXPECT_EQ(tmpl[len - 3], 't');
  EXPECT_EQ(tmpl[len - 2], 'x');
  EXPECT_EQ(tmpl[len - 1], 't');
  EXPECT_EQ(LIBC_NAMESPACE::access(tmpl, F_OK), 0);
  LIBC_NAMESPACE::close(fd);
  LIBC_NAMESPACE::unlink(tmpl);
  ::free(tmpl);
}

TEST_F(LlvmLibcMkstempsTest, OnlySixCharactersAreReplaced) {
  // The 'X' before the six that get replaced is part of the name the caller
  // chose, so it has to survive.
  char *tmpl =
      LIBC_NAMESPACE::strdup(libc_make_test_file_path("tmp_XXXXXXXX.txt"));
  const size_t len = LIBC_NAMESPACE::strlen(tmpl);
  int fd = LIBC_NAMESPACE::mkstemps(tmpl, 4);
  ASSERT_GE(fd, 0);
  EXPECT_EQ(tmpl[len - 12], 'X');
  EXPECT_EQ(tmpl[len - 11], 'X');
  LIBC_NAMESPACE::close(fd);
  LIBC_NAMESPACE::unlink(tmpl);
  ::free(tmpl);
}

TEST_F(LlvmLibcMkstempsTest, ZeroSuffixIsMkstemp) {
  char *tmpl = LIBC_NAMESPACE::strdup(libc_make_test_file_path("tmp_XXXXXX"));
  int fd = LIBC_NAMESPACE::mkstemps(tmpl, 0);
  ASSERT_GE(fd, 0);
  EXPECT_EQ(LIBC_NAMESPACE::access(tmpl, F_OK), 0);
  LIBC_NAMESPACE::close(fd);
  LIBC_NAMESPACE::unlink(tmpl);
  ::free(tmpl);
}

TEST_F(LlvmLibcMkstempsTest, MkostempsTakesOpenFlags) {
  char *tmpl =
      LIBC_NAMESPACE::strdup(libc_make_test_file_path("tmp_XXXXXX.log"));
  int fd = LIBC_NAMESPACE::mkostemps(tmpl, 4, O_CLOEXEC | O_APPEND);
  ASSERT_GE(fd, 0);
  EXPECT_EQ(LIBC_NAMESPACE::access(tmpl, F_OK), 0);
  LIBC_NAMESPACE::close(fd);
  LIBC_NAMESPACE::unlink(tmpl);
  ::free(tmpl);
}

TEST_F(LlvmLibcMkstempsTest, RejectsBadTemplates) {
  // Too short once the suffix is taken off.
  char short_template[] = "tmp_XXXXX.txt";
  EXPECT_EQ(LIBC_NAMESPACE::mkstemps(short_template, 4), -1);
  ASSERT_ERRNO_EQ(EINVAL);

  // The suffix runs past the whole template.
  char tiny[] = "XXXXXX";
  EXPECT_EQ(LIBC_NAMESPACE::mkstemps(tiny, 4), -1);
  ASSERT_ERRNO_EQ(EINVAL);

  // A suffix cannot be negative.
  char valid[] = "tmp_XXXXXX.txt";
  EXPECT_EQ(LIBC_NAMESPACE::mkstemps(valid, -1), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}
