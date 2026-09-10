//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public signal.h header.
///
//===----------------------------------------------------------------------===//

#include "include/signal.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSignalUcontextTTest, TypesAreComplete) {
  EXPECT_GT(sizeof(mcontext_t), size_t(0));
  EXPECT_GT(sizeof(ucontext_t), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(mcontext_t), size_t(256));
  EXPECT_EQ(sizeof(ucontext_t), size_t(968));
#endif
}
