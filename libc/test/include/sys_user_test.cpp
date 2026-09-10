//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/user.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/user.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysUserTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct user), size_t(0));
  EXPECT_GT(sizeof(struct user_fpregs_struct), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct user), size_t(912));
  EXPECT_EQ(sizeof(struct user_fpregs_struct), size_t(512));
#endif
}
