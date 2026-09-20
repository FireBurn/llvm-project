//===-- Unittests for free_sized and free_aligned_sized ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for free_sized and free_aligned_sized.
///
//===----------------------------------------------------------------------===//

#include "hdr/func/aligned_alloc.h"
#include "hdr/func/malloc.h"
#include "src/stdlib/free_aligned_sized.h"
#include "src/stdlib/free_sized.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcFreeSizedTest, FreesWhatMallocGave) {
  constexpr size_t SIZE = 100;
  void *ptr = ::malloc(SIZE);
  ASSERT_TRUE(ptr != nullptr);
  __builtin_memset(ptr, 0x5a, SIZE);
  LIBC_NAMESPACE::free_sized(ptr, SIZE);
}

TEST(LlvmLibcFreeSizedTest, NullIsIgnored) {
  LIBC_NAMESPACE::free_sized(nullptr, 0);
  LIBC_NAMESPACE::free_aligned_sized(nullptr, 64, 0);
}

TEST(LlvmLibcFreeSizedTest, FreesWhatAlignedAllocGave) {
  constexpr size_t SIZE = 256;
  constexpr size_t ALIGN = 64;
  void *ptr = ::aligned_alloc(ALIGN, SIZE);
  ASSERT_TRUE(ptr != nullptr);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) % ALIGN, uintptr_t(0));
  __builtin_memset(ptr, 0x5a, SIZE);
  LIBC_NAMESPACE::free_aligned_sized(ptr, ALIGN, SIZE);
}
