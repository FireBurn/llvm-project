//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for alloca being visible from stdlib.h
///
//===----------------------------------------------------------------------===//

#include "include/stdlib.h"
#include "test/UnitTest/Test.h"

#ifndef alloca
#error "alloca is not visible from <stdlib.h>"
#endif

TEST(LlvmLibcStdlibAllocaTest, AllocaGivesUsableMemory) {
  char *p = static_cast<char *>(alloca(16));
  ASSERT_TRUE(p != nullptr);
  for (int i = 0; i < 16; ++i)
    p[i] = static_cast<char>(i);
  EXPECT_EQ(p[15], char(15));
}
