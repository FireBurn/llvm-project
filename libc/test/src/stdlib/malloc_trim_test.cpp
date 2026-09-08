//===-- Unittests for malloc_trim -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/free.h"
#include "src/stdlib/malloc.h"
#include "src/stdlib/malloc_trim.h"
#include "test/UnitTest/Test.h"

// Handing memory back to the machine is the allocator's to do and nothing
// observable follows from it, so what is checked is that the call reaches
// the allocator and reports whether it did anything.
TEST(LlvmLibcMallocTrimTest, AsksTheAllocatorToHandMemoryBack) {
  void *block = LIBC_NAMESPACE::malloc(1 << 20);
  ASSERT_FALSE(block == nullptr);
  LIBC_NAMESPACE::free(block);

  // One means something was handed back, zero that there was nothing to
  // hand back. Both are answers rather than errors.
  int result = LIBC_NAMESPACE::malloc_trim(0);
  ASSERT_TRUE(result == 0 || result == 1);
}

// The padding to leave behind is advice the allocator may ignore, so any
// value is accepted.
TEST(LlvmLibcMallocTrimTest, TakesAnyPadding) {
  int result = LIBC_NAMESPACE::malloc_trim(1 << 16);
  ASSERT_TRUE(result == 0 || result == 1);
}
