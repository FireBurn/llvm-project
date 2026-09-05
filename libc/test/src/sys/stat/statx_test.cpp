//===-- Unittests for statx -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/statx.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcStatxTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcStatxTest, ReportsWhatWasAskedFor) {
  constexpr char TEXT[] = "statx";
  auto path = libc_make_test_file_path("statx.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
              Succeeds<ssize_t>(sizeof(TEXT)));
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct statx buf = {};
  ASSERT_THAT(
      LIBC_NAMESPACE::statx(AT_FDCWD, path, 0, STATX_TYPE | STATX_SIZE, &buf),
      Succeeds(0));

  // The mask says which of the fields the kernel actually filled in.
  ASSERT_NE(buf.stx_mask & STATX_SIZE, decltype(buf.stx_mask)(0));
  ASSERT_EQ(buf.stx_size, static_cast<decltype(buf.stx_size)>(sizeof(TEXT)));
  ASSERT_NE(buf.stx_mask & STATX_TYPE, decltype(buf.stx_mask)(0));
  ASSERT_TRUE(S_ISREG(buf.stx_mode));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcStatxTest, PathThatIsNotThere) {
  struct statx buf = {};
  ASSERT_THAT(LIBC_NAMESPACE::statx(AT_FDCWD, "llvm-libc-not-here", 0, 0, &buf),
              Fails(ENOENT));
}
