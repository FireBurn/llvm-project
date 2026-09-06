//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for pthread.h bringing in time.h
///
//===----------------------------------------------------------------------===//

#include "include/pthread.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_TIME_H
#error "<pthread.h> does not bring in <time.h>"
#endif

TEST(LlvmLibcPthreadTimeTest, CarriesTheHeaders) {
  struct timespec deadline = {1, 2};
  EXPECT_EQ(static_cast<long long>(deadline.tv_sec), static_cast<long long>(1));
  EXPECT_EQ(static_cast<long long>(deadline.tv_nsec), static_cast<long long>(2));
}
