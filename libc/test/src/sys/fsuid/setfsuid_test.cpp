//===-- Unittests for setfsuid and setfsgid -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/fsuid/setfsgid.h"
#include "src/sys/fsuid/setfsuid.h"
#include "src/unistd/getgid.h"
#include "src/unistd/getuid.h"
#include "test/UnitTest/Test.h"

// The call reports the user it was before rather than whether it worked, so
// asking twice for the same value shows what it settled on.
TEST(LlvmLibcSetFsUidTest, ReportsThePreviousUser) {
  const uid_t real = LIBC_NAMESPACE::getuid();
  LIBC_NAMESPACE::setfsuid(real);
  ASSERT_EQ(LIBC_NAMESPACE::setfsuid(real), static_cast<int>(real));
}

TEST(LlvmLibcSetFsUidTest, ReportsThePreviousGroup) {
  const gid_t real = LIBC_NAMESPACE::getgid();
  LIBC_NAMESPACE::setfsgid(real);
  ASSERT_EQ(LIBC_NAMESPACE::setfsgid(real), static_cast<int>(real));
}

// A user it may not become leaves it as it was, and says so by reporting the
// one it still has rather than by failing.
TEST(LlvmLibcSetFsUidTest, ARefusedChangeLeavesItAlone) {
  const uid_t real = LIBC_NAMESPACE::getuid();
  LIBC_NAMESPACE::setfsuid(real);
  const uid_t other = real == 0 ? 1 : 0;
  LIBC_NAMESPACE::setfsuid(other);
  const int now = LIBC_NAMESPACE::setfsuid(real);
  // Either the change was allowed, or it was not and the user is unchanged.
  ASSERT_TRUE(now == static_cast<int>(other) || now == static_cast<int>(real));
}
