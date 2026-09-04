//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for getgroups.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/types/gid_t.h"
#include "src/unistd/getgroups.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcGetGroupsTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcGetGroupsTest, SizeZeroIsACount) {
  // A size of zero asks how many groups there are and leaves the list alone.
  int count = LIBC_NAMESPACE::getgroups(0, nullptr);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(count, 0);

  gid_t list[NGROUPS_MAX];
  ASSERT_LE(count, NGROUPS_MAX);
  EXPECT_THAT(LIBC_NAMESPACE::getgroups(count, list), Succeeds(count));
}

TEST_F(LlvmLibcGetGroupsTest, TooSmall) {
  int count = LIBC_NAMESPACE::getgroups(0, nullptr);
  ASSERT_ERRNO_SUCCESS();
  if (count == 0)
    return; // Nothing to be too small for.

  gid_t list[NGROUPS_MAX];
  // A non-zero size smaller than the number of groups is an error.
  EXPECT_THAT(LIBC_NAMESPACE::getgroups(count - 1, list), Fails(EINVAL, -1));
}

TEST_F(LlvmLibcGetGroupsTest, NegativeSize) {
  gid_t list[1];
  EXPECT_THAT(LIBC_NAMESPACE::getgroups(-1, list), Fails(EINVAL, -1));
}
