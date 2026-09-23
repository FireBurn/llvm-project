//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the SIG_ handler values
///
//===----------------------------------------------------------------------===//

#include "include/signal.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSignalHandlerValuesTest, HandlerValuesAreTheOnesGlibcHas) {
  EXPECT_EQ(reinterpret_cast<long>(SIG_DFL), 0L);
  EXPECT_EQ(reinterpret_cast<long>(SIG_IGN), 1L);
  EXPECT_EQ(reinterpret_cast<long>(SIG_ERR), -1L);
  EXPECT_EQ(reinterpret_cast<long>(SIG_HOLD), 2L);
}
