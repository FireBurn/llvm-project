//===-- Unittests for sethostname -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/string/strlen.h"
#include "src/sys/utsname/uname.h"
#include "src/unistd/sethostname.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <sys/utsname.h>

using LlvmLibcSethostnameTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Setting the name the machine already has changes nothing, so it is the one
// value a test may pass. It still needs a privilege the test may not have,
// and EPERM is the answer where it does not.
TEST_F(LlvmLibcSethostnameTest, SettingTheNameAlreadyHeld) {
  struct utsname names;
  ASSERT_GE(LIBC_NAMESPACE::uname(&names), 0);

  size_t len = LIBC_NAMESPACE::strlen(names.nodename);
  int result = LIBC_NAMESPACE::sethostname(names.nodename, len);
  if (result != 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_ERRNO_SUCCESS();

  struct utsname after;
  ASSERT_GE(LIBC_NAMESPACE::uname(&after), 0);
  ASSERT_STREQ(after.nodename, names.nodename);
}

// A name longer than the kernel keeps room for is refused whoever asks.
TEST_F(LlvmLibcSethostnameTest, ANameTooLong) {
  char name[1024];
  for (size_t i = 0; i < sizeof(name); ++i)
    name[i] = 'a';

  ASSERT_EQ(LIBC_NAMESPACE::sethostname(name, sizeof(name)), -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EINVAL || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}
