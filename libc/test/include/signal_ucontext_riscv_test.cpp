//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ucontext_t and mcontext_t for riscv64
///
//===----------------------------------------------------------------------===//

#include "include/signal.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSignalUcontextRiscvTest, ContextHoldsTheSavedState) {
  ucontext_t context;
  __builtin_memset(&context, 0, sizeof(context));
  EXPECT_GT(sizeof(mcontext_t), size_t(0));
  context.uc_link = nullptr;
  EXPECT_TRUE(context.uc_link == nullptr);
#if defined(__riscv) && __riscv_xlen == 64
  EXPECT_EQ(sizeof(context.uc_mcontext.__gregs) / sizeof(context.uc_mcontext.__gregs[0]), size_t(32));
#endif
}
