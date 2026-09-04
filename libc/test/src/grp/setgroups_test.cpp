//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for setgroups.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/types/gid_t.h"
#include "src/grp/setgroups.h"
#include "src/unistd/getgroups.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSetGroupsTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSetGroupsTest, SetCurrentGroups) {
  int count = LIBC_NAMESPACE::getgroups(0, nullptr);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(count, 0);
  ASSERT_LE(count, NGROUPS_MAX);

  gid_t list[NGROUPS_MAX];
  ASSERT_THAT(LIBC_NAMESPACE::getgroups(count, list), Succeeds(count));

  // Setting the list to what it already is needs no privilege for an
  // unprivileged process only if the list is unchanged, which it is here.
  // A process without CAP_SETGID still gets EPERM, so both are accepted.
  int ret = LIBC_NAMESPACE::setgroups(static_cast<size_t>(count), list);
  if (ret != 0)
    ASSERT_ERRNO_EQ(EPERM);
}

TEST_F(LlvmLibcSetGroupsTest, TooManyGroups) {
  gid_t list[1] = {0};
  // A size beyond NGROUPS_MAX is rejected, but the kernel checks for
  // CAP_SETGID first, so an unprivileged process is told EPERM instead.
  EXPECT_THAT(
      LIBC_NAMESPACE::setgroups(static_cast<size_t>(NGROUPS_MAX) + 1, list),
      Fails(any_of(EINVAL, EPERM), -1));
}
