//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/in_systm.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in_systm.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetInSystmTest, TypesAreComplete) {
  EXPECT_GT(sizeof(n_short), size_t(0));
  EXPECT_GT(sizeof(n_long), size_t(0));
  EXPECT_GT(sizeof(n_time), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(n_short), size_t(2));
  EXPECT_EQ(sizeof(n_long), size_t(4));
  EXPECT_EQ(sizeof(n_time), size_t(4));
#endif
}
