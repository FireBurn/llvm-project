//===-- Unittests for vhangup ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/vhangup.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcVhangupTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;

// Hanging up the terminal needs CAP_SYS_TTY_CONFIG, which a test run as an
// ordinary user does not have. Running it as root would take the terminal
// away from whatever is watching the test, so only the refusal is checked.
TEST_F(LlvmLibcVhangupTest, RefusedWithoutThePrivilege) {
  if (LIBC_NAMESPACE::geteuid() == 0)
    return;

  ASSERT_THAT(LIBC_NAMESPACE::vhangup(), Fails(EPERM));
}
