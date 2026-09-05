//===-- Unittests for rand ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/rand.h"
#include "src/stdlib/srand.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

TEST(LlvmLibcRandTest, UnsetSeed) {
  static int vals[1000];

  for (size_t i = 0; i < 1000; ++i) {
    int val = LIBC_NAMESPACE::rand();
    ASSERT_GE(val, 0);
    ASSERT_LE(val, RAND_MAX);
    vals[i] = val;
  }

  // The C standard specifies that if 'srand' is never called it should behave
  // as if 'srand' was called with a value of 1. If we seed the value with 1 we
  // should get the same sequence as the unseeded version.
  LIBC_NAMESPACE::srand(1);
  for (size_t i = 0; i < 1000; ++i)
    ASSERT_EQ(LIBC_NAMESPACE::rand(), vals[i]);
}

TEST(LlvmLibcRandTest, SetSeed) {
  const unsigned int SEED = 12344321;
  LIBC_NAMESPACE::srand(SEED);
  const size_t NUM_RESULTS = 10;
  int results[NUM_RESULTS];
  for (size_t i = 0; i < NUM_RESULTS; ++i) {
    results[i] = LIBC_NAMESPACE::rand();
    ASSERT_GE(results[i], 0);
    ASSERT_LE(results[i], RAND_MAX);
  }

  // If the seed is set to the same value, it should give the same sequence.
  LIBC_NAMESPACE::srand(SEED);

  for (size_t i = 0; i < NUM_RESULTS; ++i) {
    int val = LIBC_NAMESPACE::rand();
    EXPECT_EQ(results[i], val);
  }
}

TEST(LlvmLibcRandTest, ZeroIsAUsableSeed) {
  // The generator has no way out of a state of all zero bits, so a seed of
  // zero must not be used as the state directly. Zero is what people reach
  // for when they want a run they can repeat, so this matters.
  LIBC_NAMESPACE::srand(0);
  int nonzero = 0;
  int previous = LIBC_NAMESPACE::rand();
  int distinct = 0;
  for (int i = 0; i < 64; ++i) {
    const int value = LIBC_NAMESPACE::rand();
    if (value != 0)
      ++nonzero;
    if (value != previous)
      ++distinct;
    previous = value;
  }
  EXPECT_GT(nonzero, 60);
  EXPECT_GT(distinct, 60);
}

TEST(LlvmLibcRandTest, EverySeedGivesItsOwnRun) {
  LIBC_NAMESPACE::srand(0);
  const int from_zero = LIBC_NAMESPACE::rand();
  LIBC_NAMESPACE::srand(1);
  const int from_one = LIBC_NAMESPACE::rand();
  LIBC_NAMESPACE::srand(2);
  const int from_two = LIBC_NAMESPACE::rand();
  EXPECT_NE(from_zero, from_one);
  EXPECT_NE(from_one, from_two);
  EXPECT_NE(from_zero, from_two);
}

TEST(LlvmLibcRandTest, TheSameSeedGivesTheSameRun) {
  LIBC_NAMESPACE::srand(12345);
  const int first = LIBC_NAMESPACE::rand();
  const int second = LIBC_NAMESPACE::rand();
  LIBC_NAMESPACE::srand(12345);
  EXPECT_EQ(LIBC_NAMESPACE::rand(), first);
  EXPECT_EQ(LIBC_NAMESPACE::rand(), second);
}
