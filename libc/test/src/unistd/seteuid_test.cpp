//===-- Unittests for seteuid and setegid ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/unistd/getegid.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/setegid.h"
#include "src/unistd/seteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSeteuidTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Asking for the identity already held is allowed whoever is asking, which
// is the one case a test can count on.
TEST_F(LlvmLibcSeteuidTest, KeepingTheIdentityAlreadyHeld) {
  uid_t uid = LIBC_NAMESPACE::geteuid();
  ASSERT_THAT(LIBC_NAMESPACE::seteuid(uid), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::geteuid(), uid);

  gid_t gid = LIBC_NAMESPACE::getegid();
  ASSERT_THAT(LIBC_NAMESPACE::setegid(gid), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::getegid(), gid);
}

// Where the caller is not root, an identity it does not hold is refused.
TEST_F(LlvmLibcSeteuidTest, AnIdentityNotHeld) {
  if (LIBC_NAMESPACE::geteuid() == 0)
    return;

  constexpr uid_t OTHER = 1;
  ASSERT_THAT(LIBC_NAMESPACE::seteuid(OTHER), Fails(EPERM));
  ASSERT_NE(LIBC_NAMESPACE::geteuid(), OTHER);
}
