//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for getresuid and getresgid.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/unistd/getegid.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/getgid.h"
#include "src/unistd/getresgid.h"
#include "src/unistd/getresuid.h"
#include "src/unistd/getuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcGetResUidTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcGetResUidTest, AgreesWithGetuid) {
  uid_t ruid = 0, euid = 0, suid = 0;
  ASSERT_THAT(LIBC_NAMESPACE::getresuid(&ruid, &euid, &suid), Succeeds(0));
  EXPECT_EQ(ruid, LIBC_NAMESPACE::getuid());
  EXPECT_EQ(euid, LIBC_NAMESPACE::geteuid());
  // Without a set-user-ID binary in play all three are the same.
  EXPECT_EQ(suid, euid);
}

TEST_F(LlvmLibcGetResUidTest, AgreesWithGetgid) {
  gid_t rgid = 0, egid = 0, sgid = 0;
  ASSERT_THAT(LIBC_NAMESPACE::getresgid(&rgid, &egid, &sgid), Succeeds(0));
  EXPECT_EQ(rgid, LIBC_NAMESPACE::getgid());
  EXPECT_EQ(egid, LIBC_NAMESPACE::getegid());
  EXPECT_EQ(sgid, egid);
}

TEST_F(LlvmLibcGetResUidTest, BadPointerFails) {
  uid_t euid = 0, suid = 0;
  EXPECT_THAT(
      LIBC_NAMESPACE::getresuid(reinterpret_cast<uid_t *>(-1), &euid, &suid),
      Fails(EFAULT, -1));
}
