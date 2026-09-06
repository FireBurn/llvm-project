//===-- Unittests for the pthread macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the pthread macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/pthread-macros.h"

TEST(LlvmLibcPthreadMacrosTest, FollowTheScalarThreadIdentifierThroughTh) {
  EXPECT_EQ(static_cast<long long>(PTHREAD_NULL), 0LL);
}
