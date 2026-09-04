//===-- Unittests for brk and sbrk ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/libc_errno.h"
#include "src/unistd/brk.h"
#include "src/unistd/sbrk.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSbrkTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {
void *const FAILED = reinterpret_cast<void *>(-1);
} // anonymous namespace

TEST_F(LlvmLibcSbrkTest, ZeroReportsTheBreak) {
  void *first = LIBC_NAMESPACE::sbrk(0);
  ASSERT_TRUE(first != FAILED);
  ASSERT_ERRNO_SUCCESS();
  // Asking twice without moving it gives the same answer.
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) == first);
}

TEST_F(LlvmLibcSbrkTest, GrowsAndReportsTheOldBreak) {
  void *before = LIBC_NAMESPACE::sbrk(0);
  ASSERT_TRUE(before != FAILED);

  constexpr intptr_t SIZE = 4096;
  void *given = LIBC_NAMESPACE::sbrk(SIZE);
  ASSERT_TRUE(given != FAILED);
  ASSERT_ERRNO_SUCCESS();
  // What comes back is where the new room starts, which is the old break.
  EXPECT_TRUE(given == before);
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) ==
              reinterpret_cast<char *>(before) + SIZE);

  // The room handed out is really there to be written.
  char *memory = reinterpret_cast<char *>(given);
  for (intptr_t i = 0; i < SIZE; ++i)
    memory[i] = static_cast<char>(i);
  for (intptr_t i = 0; i < SIZE; ++i)
    ASSERT_EQ(memory[i], static_cast<char>(i));

  // And giving it back moves the break down again.
  ASSERT_TRUE(LIBC_NAMESPACE::sbrk(-SIZE) != FAILED);
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) == before);
}

TEST_F(LlvmLibcSbrkTest, AnImpossibleGrowthFails) {
  void *before = LIBC_NAMESPACE::sbrk(0);
  ASSERT_TRUE(before != FAILED);

  // Far more than the address space holds.
  intptr_t huge = static_cast<intptr_t>(1) << (sizeof(intptr_t) * 8 - 2);
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(huge) == FAILED);
  ASSERT_ERRNO_EQ(ENOMEM);

  // A failure leaves the break where it was.
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) == before);
}

TEST_F(LlvmLibcSbrkTest, BrkSetsTheBreak) {
  void *before = LIBC_NAMESPACE::sbrk(0);
  ASSERT_TRUE(before != FAILED);

  char *wanted = reinterpret_cast<char *>(before) + 8192;
  ASSERT_EQ(LIBC_NAMESPACE::brk(wanted), 0);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) == wanted);

  ASSERT_EQ(LIBC_NAMESPACE::brk(before), 0);
  EXPECT_TRUE(LIBC_NAMESPACE::sbrk(0) == before);
}

TEST_F(LlvmLibcSbrkTest, BrkRejectsWhatItCannotDo) {
  // An address the kernel will not give out.
  EXPECT_EQ(LIBC_NAMESPACE::brk(reinterpret_cast<void *>(-4096)), -1);
  ASSERT_ERRNO_EQ(ENOMEM);
}
