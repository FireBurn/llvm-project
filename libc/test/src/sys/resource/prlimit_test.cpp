//===-- Unittests for prlimit ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_rlimit.h"
#include "src/sys/resource/getrlimit.h"
#include "src/sys/resource/prlimit.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/resource.h>

using LlvmLibcPrlimitTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// A pid of zero means this process, and asking with no new limit is a read.
TEST_F(LlvmLibcPrlimitTest, ReadsTheSameLimitAsGetrlimit) {
  struct rlimit expected;
  ASSERT_THAT(LIBC_NAMESPACE::getrlimit(RLIMIT_NOFILE, &expected), Succeeds(0));

  struct rlimit limit;
  ASSERT_THAT(LIBC_NAMESPACE::prlimit(0, RLIMIT_NOFILE, nullptr, &limit),
              Succeeds(0));
  ASSERT_EQ(limit.rlim_cur, expected.rlim_cur);
  ASSERT_EQ(limit.rlim_max, expected.rlim_max);
}

// Setting the limit already in force is allowed, and hands back what it
// replaced.
TEST_F(LlvmLibcPrlimitTest, SettingTheLimitAlreadyInForce) {
  struct rlimit limit;
  ASSERT_THAT(LIBC_NAMESPACE::prlimit(0, RLIMIT_NOFILE, nullptr, &limit),
              Succeeds(0));

  struct rlimit previous;
  ASSERT_THAT(LIBC_NAMESPACE::prlimit(0, RLIMIT_NOFILE, &limit, &previous),
              Succeeds(0));
  ASSERT_EQ(previous.rlim_cur, limit.rlim_cur);
}

TEST_F(LlvmLibcPrlimitTest, AResourceThatIsNotOne) {
  struct rlimit limit;
  ASSERT_THAT(LIBC_NAMESPACE::prlimit(0, -1, nullptr, &limit), Fails(EINVAL));
}
