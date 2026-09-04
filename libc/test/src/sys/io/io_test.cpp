//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ioperm and iopl
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/io/ioperm.h"
#include "src/sys/io/iopl.h"
#include "src/unistd/geteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcIoTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Reaching the machine's io ports needs CAP_SYS_RAWIO, which a test does not
// have. Taking it where the test does run as root would leave the process
// able to write to hardware, so only the refusal is checked.
TEST_F(LlvmLibcIoTest, RefusedWithoutThePrivilege) {
  if (LIBC_NAMESPACE::geteuid() == 0)
    return;

  // A kernel built without CONFIG_X86_IOPL_IOPERM has neither call.
  ASSERT_EQ(LIBC_NAMESPACE::ioperm(0x378, 3, 1), -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EPERM || err == ENOSYS);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::iopl(3), -1);
  err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EPERM || err == ENOSYS);
  LIBC_NAMESPACE::libc_errno = 0;
}

// A level above three is not a level at all, and is refused before any
// privilege is looked at.
TEST_F(LlvmLibcIoTest, ALevelThatIsNotOne) {
  int result = LIBC_NAMESPACE::iopl(4);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EINVAL || err == EPERM || err == ENOSYS);
  LIBC_NAMESPACE::libc_errno = 0;
}
