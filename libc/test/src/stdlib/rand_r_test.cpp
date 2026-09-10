//===-- Unittests for rand_r ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/macros/config.h"
#include "src/stdlib/rand_r.h"
#include "test/UnitTest/Test.h"

#include "hdr/stdlib_macros.h"

TEST(LlvmLibcRandRTest, ValuesAreInRange) {
  unsigned int seed = 12345;
  for (int i = 0; i < 1000; ++i) {
    int value = LIBC_NAMESPACE::rand_r(&seed);
    ASSERT_GE(value, 0);
    ASSERT_LE(value, RAND_MAX);
  }
}

TEST(LlvmLibcRandRTest, SameSeedGivesSameRun) {
  unsigned int first = 7;
  unsigned int second = 7;
  for (int i = 0; i < 100; ++i)
    ASSERT_EQ(LIBC_NAMESPACE::rand_r(&first), LIBC_NAMESPACE::rand_r(&second));
}

TEST(LlvmLibcRandRTest, StateIsTheCallersOwn) {
  unsigned int first = 1;
  unsigned int second = 2;
  LIBC_NAMESPACE::rand_r(&first);
  unsigned int after_one = first;
  LIBC_NAMESPACE::rand_r(&second);
  ASSERT_EQ(first, after_one);
}

// The generator has no way out of a state of all zero bits, so a caller who
// starts from zero, as one counting up from it does, must still get a run.
TEST(LlvmLibcRandRTest, ZeroIsNotAStandstill) {
  unsigned int seed = 0;
  int first = LIBC_NAMESPACE::rand_r(&seed);
  int second = LIBC_NAMESPACE::rand_r(&seed);
  ASSERT_NE(seed, 0u);
  ASSERT_NE(first, second);
}
