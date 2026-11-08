//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Tests that Android's ucontext_t has bionic's second name for the signal
/// mask.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/signal.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidUcontextTest, SigmaskHasA64BitName) {
  ucontext_t context = {};
  EXPECT_EQ(sizeof(context.uc_sigmask64), sizeof(context.uc_sigmask));
  EXPECT_TRUE(static_cast<void *>(&context.uc_sigmask64) ==
              static_cast<void *>(&context.uc_sigmask));
}
