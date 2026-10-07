//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/socket.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/socket.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysSocketLoffTTest, TypesAreComplete) {
  EXPECT_GT(sizeof(loff_t), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(loff_t), size_t(8));
#endif
}
