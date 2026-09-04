//===-- Unittests for the random family -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/stdlib/initstate.h"
#include "src/stdlib/random.h"
#include "src/stdlib/setstate.h"
#include "src/stdlib/srandom.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcRandomTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcRandomTest, InRange) {
  LIBC_NAMESPACE::srandom(1);
  for (int i = 0; i < 1000; ++i) {
    long value = LIBC_NAMESPACE::random();
    ASSERT_GE(value, 0L);
    ASSERT_LE(value, 2147483647L);
  }
}

TEST_F(LlvmLibcRandomTest, TheSameSeedGivesTheSameSequence) {
  LIBC_NAMESPACE::srandom(12345);
  long first[8];
  for (int i = 0; i < 8; ++i)
    first[i] = LIBC_NAMESPACE::random();

  LIBC_NAMESPACE::srandom(12345);
  for (int i = 0; i < 8; ++i)
    EXPECT_EQ(LIBC_NAMESPACE::random(), first[i]);
}

TEST_F(LlvmLibcRandomTest, DifferentSeedsGiveDifferentSequences) {
  LIBC_NAMESPACE::srandom(1);
  long first = LIBC_NAMESPACE::random();
  LIBC_NAMESPACE::srandom(2);
  EXPECT_NE(LIBC_NAMESPACE::random(), first);
}

TEST_F(LlvmLibcRandomTest, KnownSequence) {
  // The sequence for seed 1 is the one every implementation of this
  // generator produces, so it pins the arithmetic rather than just its
  // self consistency.
  LIBC_NAMESPACE::srandom(1);
  EXPECT_EQ(LIBC_NAMESPACE::random(), 1804289383L);
  EXPECT_EQ(LIBC_NAMESPACE::random(), 846930886L);
  EXPECT_EQ(LIBC_NAMESPACE::random(), 1681692777L);
  EXPECT_EQ(LIBC_NAMESPACE::random(), 1714636915L);
}

TEST_F(LlvmLibcRandomTest, SeedZeroIsTreatedAsOne) {
  LIBC_NAMESPACE::srandom(0);
  long from_zero = LIBC_NAMESPACE::random();
  LIBC_NAMESPACE::srandom(1);
  EXPECT_EQ(LIBC_NAMESPACE::random(), from_zero);
}

TEST_F(LlvmLibcRandomTest, EachStateSizeIsItsOwnGenerator) {
  static char small[8];
  static char large[256];

  ASSERT_TRUE(LIBC_NAMESPACE::initstate(42, small, sizeof(small)) != nullptr);
  long from_small = LIBC_NAMESPACE::random();

  ASSERT_TRUE(LIBC_NAMESPACE::initstate(42, large, sizeof(large)) != nullptr);
  long from_large = LIBC_NAMESPACE::random();

  // The same seed in a different size of table is a different sequence.
  EXPECT_NE(from_small, from_large);
}

TEST_F(LlvmLibcRandomTest, SetstateTakesUpWhereATableWasLeft) {
  static char first[128];
  static char second[128];

  LIBC_NAMESPACE::initstate(7, first, sizeof(first));
  long a1 = LIBC_NAMESPACE::random();
  long a2 = LIBC_NAMESPACE::random();

  // Move to another table, which is its own sequence.
  char *previous = LIBC_NAMESPACE::initstate(99, second, sizeof(second));
  ASSERT_TRUE(previous != nullptr);
  LIBC_NAMESPACE::random();

  // Going back to the first one carries on from where it stopped rather
  // than starting over.
  ASSERT_TRUE(LIBC_NAMESPACE::setstate(first) != nullptr);
  long a3 = LIBC_NAMESPACE::random();
  EXPECT_NE(a3, a1);
  EXPECT_NE(a3, a2);

  // And seeding it again does start it over.
  LIBC_NAMESPACE::srandom(7);
  EXPECT_EQ(LIBC_NAMESPACE::random(), a1);
}

TEST_F(LlvmLibcRandomTest, BadArguments) {
  static char state[128];
  EXPECT_TRUE(LIBC_NAMESPACE::initstate(1, nullptr, sizeof(state)) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  // A table too small to be any of the kinds.
  EXPECT_TRUE(LIBC_NAMESPACE::initstate(1, state, 4) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_TRUE(LIBC_NAMESPACE::setstate(nullptr) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
}
