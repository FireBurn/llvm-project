//===-- Unittests for unshare and setns -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/sched/setns.h"
#include "src/sched/unshare.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sched.h>

using LlvmLibcNamespaceTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Asking to unshare nothing is allowed whoever is asking, and changes
// nothing, so it is the one call a test may make that has to succeed.
TEST_F(LlvmLibcNamespaceTest, UnsharingNothing) {
  ASSERT_THAT(LIBC_NAMESPACE::unshare(0), Succeeds(0));
}

// Leaving the mount namespace needs CAP_SYS_ADMIN, which a test run as an
// ordinary user does not have.
TEST_F(LlvmLibcNamespaceTest, UnsharingWhatNeedsAPrivilege) {
  int result = LIBC_NAMESPACE::unshare(CLONE_NEWNS);
  if (result != 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
  }
}

TEST_F(LlvmLibcNamespaceTest, FlagsThatAreNotFlags) {
  ASSERT_THAT(LIBC_NAMESPACE::unshare(0x1), Fails(EINVAL));
}

// Joining the namespace this process is already in changes nothing, and is
// the only join a test can make; it still needs the privilege on most
// machines, and EPERM is the answer where it is not there.
TEST_F(LlvmLibcNamespaceTest, JoiningTheNamespaceAlreadyIn) {
  int fd = LIBC_NAMESPACE::open("/proc/self/ns/mnt", O_RDONLY);
  if (fd < 0) {
    // A machine need not have /proc mounted.
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  int result = LIBC_NAMESPACE::setns(fd, CLONE_NEWNS);
  if (result != 0) {
    int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
    ASSERT_TRUE(err == EPERM || err == EINVAL);
    LIBC_NAMESPACE::libc_errno = 0;
  }

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcNamespaceTest, DescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::setns(-1, 0), Fails(EBADF));
}
