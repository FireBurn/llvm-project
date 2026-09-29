//===-- Unittests for setpgrp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getpgrp.h"
#include "src/unistd/getpid.h"
#include "src/unistd/setpgrp.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSetPgrpTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSetPgrpTest, LeadsItsOwnGroup) {
  ASSERT_THAT(LIBC_NAMESPACE::setpgrp(), Succeeds());
  ASSERT_EQ(LIBC_NAMESPACE::getpgrp(), LIBC_NAMESPACE::getpid());
}
