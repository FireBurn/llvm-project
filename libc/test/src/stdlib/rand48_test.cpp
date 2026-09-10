//===-- Unittests for the 48 bit random family ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/drand48.h"
#include "src/stdlib/erand48.h"
#include "src/stdlib/jrand48.h"
#include "src/stdlib/lcong48.h"
#include "src/stdlib/lrand48.h"
#include "src/stdlib/mrand48.h"
#include "src/stdlib/nrand48.h"
#include "src/stdlib/seed48.h"
#include "src/stdlib/srand48.h"
#include "test/UnitTest/Test.h"

// The sequence is fixed by the standard, so a seed of one has an answer that
// can be written down. These are what the same calls give on a system whose
// generator is right, which is the only thing that makes the family useful:
// the same seed has to give the same numbers everywhere.
TEST(LlvmLibcRand48Test, SeedOneGivesTheKnownSequence) {
  LIBC_NAMESPACE::srand48(1);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), 89400484L);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), 976015093L);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), 1792756325L);
}

// The rest of the family reads the same step differently, so one seed pins
// all of them down at once.
TEST(LlvmLibcRand48Test, TheFamilyAgreesOnTheSameStep) {
  LIBC_NAMESPACE::srand48(1);
  ASSERT_EQ(LIBC_NAMESPACE::mrand48(), 178800969L);

  unsigned short xsubi[3] = {0x330e, 0, 0};
  ASSERT_EQ(LIBC_NAMESPACE::nrand48(xsubi), 366850414L);
}

// Seeding again starts the same sequence over.
TEST(LlvmLibcRand48Test, SeedingAgainStartsOver) {
  LIBC_NAMESPACE::srand48(42);
  long first = LIBC_NAMESPACE::lrand48();
  long second = LIBC_NAMESPACE::lrand48();
  LIBC_NAMESPACE::srand48(42);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), first);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), second);
}

// The calls which carry their own sequence leave the shared one alone.
TEST(LlvmLibcRand48Test, OwnSequenceIsSeparate) {
  LIBC_NAMESPACE::srand48(7);
  long shared = LIBC_NAMESPACE::lrand48();

  unsigned short mine[3] = {0x330e, 0, 1};
  LIBC_NAMESPACE::nrand48(mine);

  LIBC_NAMESPACE::srand48(7);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), shared);
}

// nrand48 never answers with a negative number; jrand48 uses the whole range
// either side of zero, which is the difference between them.
TEST(LlvmLibcRand48Test, SignedAndUnsignedRanges) {
  unsigned short xsubi[3] = {0x330e, 0, 0};
  for (int i = 0; i < 200; ++i) {
    long v = LIBC_NAMESPACE::nrand48(xsubi);
    ASSERT_GE(v, 0L);
    ASSERT_LT(v, 2147483648L);
  }

  unsigned short other[3] = {0x330e, 0, 0};
  bool saw_negative = false;
  for (int i = 0; i < 200; ++i) {
    long v = LIBC_NAMESPACE::jrand48(other);
    ASSERT_GE(v, -2147483648L);
    ASSERT_LT(v, 2147483648L);
    if (v < 0)
      saw_negative = true;
  }
  ASSERT_TRUE(saw_negative);
}

// The doubles stay inside the half open unit interval.
TEST(LlvmLibcRand48Test, DoublesAreInTheUnitInterval) {
  LIBC_NAMESPACE::srand48(3);
  for (int i = 0; i < 200; ++i) {
    double v = LIBC_NAMESPACE::drand48();
    ASSERT_TRUE(v >= 0.0);
    ASSERT_TRUE(v < 1.0);
  }

  unsigned short xsubi[3] = {1, 2, 3};
  for (int i = 0; i < 200; ++i) {
    double v = LIBC_NAMESPACE::erand48(xsubi);
    ASSERT_TRUE(v >= 0.0);
    ASSERT_TRUE(v < 1.0);
  }
}

// seed48 hands back what the sequence was, so a caller can put it back and
// carry on where it left off.
TEST(LlvmLibcRand48Test, SeedFortyEightGivesBackTheOldSequence) {
  LIBC_NAMESPACE::srand48(11);
  long expected = LIBC_NAMESPACE::lrand48();

  LIBC_NAMESPACE::srand48(11);
  unsigned short replacement[3] = {1, 2, 3};
  unsigned short *previous = LIBC_NAMESPACE::seed48(replacement);
  unsigned short saved[3] = {previous[0], previous[1], previous[2]};

  LIBC_NAMESPACE::seed48(saved);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), expected);
}

// lcong48 replaces the multiplier and the addend as well, so the sequence
// after it is not the standard one.
TEST(LlvmLibcRand48Test, LcongReplacesTheWholeGenerator) {
  LIBC_NAMESPACE::srand48(5);
  long standard = LIBC_NAMESPACE::lrand48();

  unsigned short param[7] = {0x330e, 0, 5, 0xabcd, 0x1234, 0x5, 0x7};
  LIBC_NAMESPACE::lcong48(param);
  ASSERT_NE(LIBC_NAMESPACE::lrand48(), standard);

  // srand48 puts the standard multiplier and addend back.
  LIBC_NAMESPACE::srand48(5);
  ASSERT_EQ(LIBC_NAMESPACE::lrand48(), standard);
}
