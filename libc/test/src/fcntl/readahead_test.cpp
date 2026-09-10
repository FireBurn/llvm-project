//===-- Unittests for readahead -------------------------------------------===//
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
#include "src/fcntl/readahead.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcReadaheadTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Nothing comes back but whether the request was made, so what can be
// checked is that a file which is open is accepted.
TEST_F(LlvmLibcReadaheadTest, AsksForAnOpenFileToBeCached) {
  auto path = libc_make_test_file_path("readahead.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_THAT(LIBC_NAMESPACE::write(fd, "cached", 6), Succeeds<ssize_t>(6));
  ASSERT_THAT(LIBC_NAMESPACE::readahead(fd, 0, 4096), Succeeds<ssize_t>(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// A descriptor which is not open is refused.
TEST_F(LlvmLibcReadaheadTest, RefusesADescriptorThatIsNotOpen) {
  ASSERT_THAT(LIBC_NAMESPACE::readahead(-1, 0, 1), Fails(EBADF));
}
