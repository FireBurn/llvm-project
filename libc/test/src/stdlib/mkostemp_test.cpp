//===-- Unittests for mkostemp --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/fcntl/fcntl.h"
#include "src/stdlib/mkostemp.h"
#include "src/string/strncmp.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcMkostempTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// mkostemp is mkstemp with flags for the descriptor it opens, which is what
// it is for: a temporary file that is not passed on across an exec.
TEST_F(LlvmLibcMkostempTest, OpensAFileWithTheGivenFlags) {
  char name[] = "mkostemp-XXXXXX";
  int fd = LIBC_NAMESPACE::mkostemp(name, O_CLOEXEC);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  // The six placeholders were replaced, and the rest of the name was left.
  ASSERT_EQ(LIBC_NAMESPACE::strncmp(name, "mkostemp-", 9), 0);
  ASSERT_NE(LIBC_NAMESPACE::strncmp(name + 9, "XXXXXX", 6), 0);

  int flags = LIBC_NAMESPACE::fcntl(fd, F_GETFD);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(flags & FD_CLOEXEC, FD_CLOEXEC);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(name), Succeeds(0));
}

TEST_F(LlvmLibcMkostempTest, NoFlagsLeavesTheDescriptorAlone) {
  char name[] = "mkostemp-plain-XXXXXX";
  int fd = LIBC_NAMESPACE::mkostemp(name, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  int flags = LIBC_NAMESPACE::fcntl(fd, F_GETFD);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(flags & FD_CLOEXEC, 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(name), Succeeds(0));
}

// A name not ending in six placeholders is not a template.
TEST_F(LlvmLibcMkostempTest, ANameThatIsNotATemplate) {
  char name[] = "mkostemp-XXXXX";
  ASSERT_THAT(LIBC_NAMESPACE::mkostemp(name, 0), Fails(EINVAL));
}
