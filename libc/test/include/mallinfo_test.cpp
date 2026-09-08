//===-- Unittests for struct mallinfo and struct mallinfo2 ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-types/struct_mallinfo.h"
#include "include/llvm-libc-types/struct_mallinfo2.h"
#include "test/UnitTest/Test.h"

// The allocator fills these in, so they have to be laid out the way scudo's
// wrappers and glibc lay them out: ten counters in a row, each an int in the
// older form and a size_t in the newer one.
TEST(LlvmLibcMallinfoTest, TenCountersInARow) {
  EXPECT_EQ(sizeof(struct mallinfo), 10 * sizeof(int));
  EXPECT_EQ(__builtin_offsetof(struct mallinfo, arena), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct mallinfo, keepcost), 9 * sizeof(int));

  EXPECT_EQ(sizeof(struct mallinfo2), 10 * sizeof(size_t));
  EXPECT_EQ(__builtin_offsetof(struct mallinfo2, arena), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct mallinfo2, keepcost), 9 * sizeof(size_t));
}
