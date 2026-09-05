//===-- Unittests for getdtablesize ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_rlimit.h"
#include "src/sys/resource/getrlimit.h"
#include "src/unistd/getdtablesize.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <sys/resource.h>

using LlvmLibcGetdtablesizeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The answer is the soft limit on open descriptors, which is what the
// process is actually held to.
TEST_F(LlvmLibcGetdtablesizeTest, MatchesTheSoftLimit) {
  struct rlimit limit;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_NOFILE, &limit), 0);

  int size = LIBC_NAMESPACE::getdtablesize();
  ASSERT_GT(size, 0);
  ASSERT_EQ(static_cast<rlim_t>(size), limit.rlim_cur);
}
