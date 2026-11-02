//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ucontext_t and mcontext_t
///
//===----------------------------------------------------------------------===//

#include "include/signal.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSignalUcontextAarch64Test, ContextHoldsTheSavedState) {
  ucontext_t context;
  __builtin_memset(&context, 0, sizeof(context));
  EXPECT_GT(sizeof(mcontext_t), size_t(0));
  EXPECT_EQ(sizeof(context.uc_mcontext), sizeof(mcontext_t));
  EXPECT_GT(sizeof(context.uc_sigmask), size_t(0));
  context.uc_stack.ss_size = 4096;
  EXPECT_EQ(context.uc_stack.ss_size, size_t(4096));
#if defined(__aarch64__)
  EXPECT_EQ(sizeof(context.uc_mcontext.regs) / sizeof(context.uc_mcontext.regs[0]), size_t(31));
#endif
}
