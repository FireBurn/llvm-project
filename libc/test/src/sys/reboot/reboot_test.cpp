//===-- Unittests for reboot ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/sys/reboot/reboot.h"
#include "src/unistd/geteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/reboot.h>

using LlvmLibcRebootTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;

// Every command this call takes either stops the machine or changes how it
// answers the keyboard, so the only one a test may make is the one that is
// refused. RB_ENABLE_CAD is the mildest of them and is still privileged.
TEST_F(LlvmLibcRebootTest, RefusedWithoutThePrivilege) {
  if (LIBC_NAMESPACE::geteuid() == 0)
    return;

  ASSERT_THAT(LIBC_NAMESPACE::reboot(RB_ENABLE_CAD), Fails(EPERM));
}
