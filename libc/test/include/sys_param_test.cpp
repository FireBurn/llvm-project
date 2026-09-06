//===-- Unittests for sys/param.h macros ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/sys-param-macros.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysParamTest, Rounding) {
  EXPECT_EQ(howmany(7, 4), 2);
  EXPECT_EQ(roundup(7, 4), 8);
  EXPECT_TRUE(powerof2(8));
  EXPECT_FALSE(powerof2(6));
  EXPECT_EQ(MIN(3, 5), 3);
  EXPECT_EQ(MAX(3, 5), 5);
}

TEST(LlvmLibcSysParamTest, BitArray) {
  unsigned char bits[2] = {0, 0};

  setbit(bits, 3);
  setbit(bits, 9);
  EXPECT_EQ(int(bits[0]), 1 << 3);
  EXPECT_EQ(int(bits[1]), 1 << 1);
  EXPECT_TRUE(isset(bits, 3) != 0);
  EXPECT_TRUE(isset(bits, 9) != 0);
  EXPECT_TRUE(isclr(bits, 4) != 0);

  clrbit(bits, 3);
  EXPECT_TRUE(isclr(bits, 3) != 0);
  EXPECT_TRUE(isset(bits, 9) != 0);

  // The index is taken to be in units of NBBY bits whatever the element type.
  EXPECT_EQ(NBBY, 8);
}
