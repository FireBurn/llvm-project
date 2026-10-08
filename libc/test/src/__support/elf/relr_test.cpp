//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for DT_RELR relocations
///
//===----------------------------------------------------------------------===//

#include "src/__support/elf/relocation.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::elf::RelrTable;

namespace {

constexpr size_t WORDS = 200;

// Every word holds its own index plus 1000, so what was relocated, and by how
// much, can be told from what was not.
void fill(uintptr_t (&image)[WORDS]) {
  for (size_t i = 0; i < WORDS; ++i)
    image[i] = 1000 + i;
}

} // namespace

TEST(LlvmLibcElfRelrTest, AddressesAndBitmaps) {
  uintptr_t image[WORDS];
  fill(image);
  const uintptr_t base = reinterpret_cast<uintptr_t>(image);

  // An address, a bitmap for the words after it, another address, and two
  // bitmaps, the second of which starts 63 words on from the first.
  const uintptr_t table[] = {
      3 * sizeof(uintptr_t), // word 3
      (0b10110 << 1) | 1,    // words 5, 6 and 8
      70 * sizeof(uintptr_t), // word 70
      (0b101 << 1) | 1,       // words 71 and 73... not 72
      (0b101 << 1) | 1,       // 63 words past the last bitmap: 134 and 136
  };
  const RelrTable relr(table, sizeof(table));
  EXPECT_EQ(relr.size(), sizeof(table) / sizeof(table[0]));

  EXPECT_EQ(relr.apply(base), size_t(9));

  bool relocated[WORDS] = {};
  constexpr size_t RELOCATED[] = {3, 5, 6, 8, 70, 71, 73, 134, 136};
  for (size_t i : RELOCATED)
    relocated[i] = true;
  for (size_t i = 0; i < WORDS; ++i)
    EXPECT_EQ(image[i], 1000 + i + (relocated[i] ? base : 0));
}

TEST(LlvmLibcElfRelrTest, EmptyTableChangesNothing) {
  uintptr_t image[WORDS];
  fill(image);
  const RelrTable relr;
  EXPECT_TRUE(relr.empty());
  EXPECT_EQ(relr.apply(reinterpret_cast<uintptr_t>(image)), size_t(0));
  for (size_t i = 0; i < WORDS; ++i)
    EXPECT_EQ(image[i], 1000 + i);
}
