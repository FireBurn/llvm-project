//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for acct
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/unistd/acct.h"
#include "src/unistd/geteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LlvmLibcAcctTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Turning accounting on is a privileged act, and turning it on for a file
// that is not there fails whatever the privilege, so only the failures can be
// relied on to hold on any machine the tests run on.
TEST_F(LlvmLibcAcctTest, FailsWithoutPrivilegeOrWithoutAFile) {
  if (LIBC_NAMESPACE::geteuid() != 0) {
    ASSERT_THAT(LIBC_NAMESPACE::acct("/nonexistent/acct"), Fails(EPERM));
    return;
  }
  ASSERT_THAT(LIBC_NAMESPACE::acct("/nonexistent/acct"), Fails(ENOENT));
}
