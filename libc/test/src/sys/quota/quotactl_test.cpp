//===-- Unittests for quotactl and quotactl_fd ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/quota/quotactl.h"
#include "src/sys/quota/quotactl_fd.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/quota.h>

using LlvmLibcQuotactlTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// A device that is not one is refused before any privilege is looked at, so
// this says the command word was packed the way the kernel reads it.
TEST_F(LlvmLibcQuotactlTest, ADeviceThatIsNotThere) {
  char buf[64] = {};
  int result = LIBC_NAMESPACE::quotactl(QCMD(Q_GETQUOTA, USRQUOTA),
                                        "/dev/llvm-libc-not-here", 0, buf);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == ENOENT || err == EPERM || err == ENOTBLK || err == ENOSYS);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcQuotactlTest, DescriptorThatIsNotOne) {
  char buf[64] = {};
  ASSERT_EQ(LIBC_NAMESPACE::quotactl_fd(-1, QCMD(Q_GETQUOTA, USRQUOTA), 0, buf),
            -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EBADF || err == ENOSYS);
  LIBC_NAMESPACE::libc_errno = 0;
}
