//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/time.h bringing in sys/select.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/time.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_SYS_SELECT_H
#error "<sys/time.h> does not bring in <sys/select.h>"
#endif

TEST(LlvmLibcSysTimeSelectTest, CarriesTheHeaders) {
  fd_set set;
  FD_ZERO(&set);
  EXPECT_EQ(static_cast<long long>(FD_ISSET(0, &set)), 0LL);
}
