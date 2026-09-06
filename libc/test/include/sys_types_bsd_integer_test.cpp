//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/types.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/types.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysTypesBsdIntegerTest, TypesAreComplete) {
  EXPECT_GT(sizeof(u_int64_t), size_t(0));
  EXPECT_GT(sizeof(quad_t), size_t(0));
  EXPECT_GT(sizeof(u_quad_t), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(u_int64_t), size_t(8));
  EXPECT_EQ(sizeof(quad_t), size_t(8));
  EXPECT_EQ(sizeof(u_quad_t), size_t(8));
#endif
}
