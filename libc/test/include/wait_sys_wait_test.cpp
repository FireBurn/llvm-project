//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for wait.h bringing in sys/wait.h
///
//===----------------------------------------------------------------------===//

#include "include/wait.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_SYS_WAIT_H
#error "<wait.h> does not bring in <sys/wait.h>"
#endif

TEST(LlvmLibcWaitSysWaitTest, CarriesTheHeaders) {
  EXPECT_TRUE(WIFEXITED(0));
}
