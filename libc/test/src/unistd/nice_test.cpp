//===-- Unittests for nice ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/resource/getpriority.h"
#include "src/unistd/nice.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <sys/resource.h>

using LlvmLibcNiceTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// nice reports the new value, which is the old one plus the increment. It
// also reports -1 for a niceness of -1, so errno is what tells the two
// apart and it has to be cleared before the call.
TEST_F(LlvmLibcNiceTest, AskingForNothingLeavesItAlone) {
  int before = LIBC_NAMESPACE::getpriority(PRIO_PROCESS, 0);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::nice(0), before);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcNiceTest, AskingToBeNicer) {
  int before = LIBC_NAMESPACE::getpriority(PRIO_PROCESS, 0);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::nice(1), before + 1);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(LIBC_NAMESPACE::getpriority(PRIO_PROCESS, 0), before + 1);
}

// Lowering niceness needs a privilege a test does not have, and where it is
// refused the value is left alone. Linux says EACCES for this rather than
// the EPERM POSIX names.
TEST_F(LlvmLibcNiceTest, AskingToBeLessNice) {
  int before = LIBC_NAMESPACE::getpriority(PRIO_PROCESS, 0);
  ASSERT_ERRNO_SUCCESS();

  int result = LIBC_NAMESPACE::nice(-1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  if (err == EACCES || err == EPERM) {
    ASSERT_EQ(result, -1);
    ASSERT_EQ(LIBC_NAMESPACE::getpriority(PRIO_PROCESS, 0), before);
    LIBC_NAMESPACE::libc_errno = 0;
  } else {
    ASSERT_ERRNO_SUCCESS();
    ASSERT_EQ(result, before - 1);
  }
}
