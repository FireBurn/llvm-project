//===-- Unittests for getdomainname and setdomainname ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/unistd/getdomainname.h"
#include "src/unistd/setdomainname.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetDomainNameTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcGetDomainNameTest, ReadsTheName) {
  char name[256];
  for (char &c : name)
    c = '@';
  ASSERT_EQ(LIBC_NAMESPACE::getdomainname(name, sizeof(name)), 0);
  // Whatever it is, it is a string.
  bool terminated = false;
  for (size_t i = 0; i < sizeof(name); ++i)
    if (name[i] == '\0') {
      terminated = true;
      break;
    }
  ASSERT_TRUE(terminated);
}

// A name that will not fit is refused rather than cut short, so that what is
// handed back is never a piece of a name.
TEST_F(LlvmLibcGetDomainNameTest, TooSmallIsRefused) {
  char full[256];
  ASSERT_EQ(LIBC_NAMESPACE::getdomainname(full, sizeof(full)), 0);
  size_t length = 0;
  while (full[length] != '\0')
    ++length;

  char small[1];
  small[0] = '@';
  if (length > 0) {
    ASSERT_EQ(LIBC_NAMESPACE::getdomainname(small, 1), -1);
    ASSERT_ERRNO_EQ(EINVAL);
    ASSERT_EQ(small[0], '@');
  }
}

TEST_F(LlvmLibcGetDomainNameTest, NoBufferIsRefused) {
  ASSERT_EQ(LIBC_NAMESPACE::getdomainname(nullptr, 16), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}

// Setting it needs privilege this test does not have, so what is checked is
// that the refusal comes back rather than something else.
TEST_F(LlvmLibcGetDomainNameTest, SettingWithoutPrivilegeIsRefused) {
  ASSERT_EQ(LIBC_NAMESPACE::setdomainname(nullptr, 1), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}
