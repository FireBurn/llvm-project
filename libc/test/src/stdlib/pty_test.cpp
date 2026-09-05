//===-- Unittests for the pseudo terminal calls ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/libc_errno.h"
#include "src/stdlib/grantpt.h"
#include "src/stdlib/posix_openpt.h"
#include "src/stdlib/ptsname.h"
#include "src/stdlib/ptsname_r.h"
#include "src/stdlib/unlockpt.h"
#include "src/string/strlen.h"
#include "src/string/strncmp.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPtyTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// The four calls are used in the order POSIX gives them: open the first end,
// set the ownership of the second, unlock it, and ask what it is called.
TEST_F(LlvmLibcPtyTest, OpensATerminalAndNamesTheOtherEnd) {
  int fd = LIBC_NAMESPACE::posix_openpt(O_RDWR | O_NOCTTY);
  if (fd < 0) {
    // A machine need not have terminals to hand out at all.
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_GT(fd, 0);

  ASSERT_THAT(LIBC_NAMESPACE::grantpt(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlockpt(fd), Succeeds(0));

  char *name = LIBC_NAMESPACE::ptsname(fd);
  ASSERT_FALSE(name == nullptr);
  ASSERT_ERRNO_SUCCESS();
  // Every one of them is under /dev/pts.
  ASSERT_EQ(LIBC_NAMESPACE::strncmp(name, "/dev/pts/", 9), 0);

  // The reentrant form writes the same name into the caller's buffer.
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::ptsname_r(fd, buf, sizeof(buf)), 0);
  ASSERT_EQ(LIBC_NAMESPACE::strncmp(buf, name, sizeof(buf)), 0);

  // A buffer too small for the name is reported rather than truncated into.
  char small[4];
  ASSERT_EQ(LIBC_NAMESPACE::ptsname_r(fd, small, sizeof(small)), ERANGE);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

// None of them means anything for a descriptor that is not a terminal.
TEST_F(LlvmLibcPtyTest, DescriptorThatIsNotATerminal) {
  ASSERT_THAT(LIBC_NAMESPACE::grantpt(-1), Fails(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::unlockpt(-1), Fails(EBADF));

  ASSERT_TRUE(LIBC_NAMESPACE::ptsname(-1) == nullptr);
  ASSERT_ERRNO_EQ(EBADF);
  LIBC_NAMESPACE::libc_errno = 0;

  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::ptsname_r(-1, buf, sizeof(buf)), EBADF);
}
