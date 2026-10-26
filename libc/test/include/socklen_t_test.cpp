//===-- Unittests for socklen_t ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for socklen_t.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/socklen_t.h"

// The kernel reads and writes a socklen_t through a pointer to a 32 bit int.
TEST(LlvmLibcSocklenTTest, IsUnsigned32Bits) {
  ASSERT_EQ(sizeof(socklen_t), size_t(4));
  ASSERT_TRUE(socklen_t(-1) > 0);
  ASSERT_TRUE(__is_same(socklen_t, unsigned int));
}
