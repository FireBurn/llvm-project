//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the arc4random functions.
///
//===----------------------------------------------------------------------===//

#include "hdr/stdint_proxy.h"
#include "src/stdlib/arc4random.h"
#include "src/stdlib/arc4random_buf.h"
#include "src/stdlib/arc4random_uniform.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcArc4randomTest, BufFillsEveryByte) {
  // A buffer of zeros stays zero with a chance of 2^-2048.
  unsigned char buf[256] = {};
  LIBC_NAMESPACE::arc4random_buf(buf, sizeof(buf));
  bool any = false;
  for (unsigned char byte : buf)
    any = any || byte != 0;
  ASSERT_TRUE(any);

  // Nothing is written past the end.
  unsigned char guarded[16];
  for (unsigned char &byte : guarded)
    byte = 0xa5;
  LIBC_NAMESPACE::arc4random_buf(guarded + 4, 8);
  for (int i = 0; i < 4; ++i)
    ASSERT_EQ(guarded[i], static_cast<unsigned char>(0xa5));
  for (int i = 12; i < 16; ++i)
    ASSERT_EQ(guarded[i], static_cast<unsigned char>(0xa5));
  LIBC_NAMESPACE::arc4random_buf(guarded, 0);
}

TEST(LlvmLibcArc4randomTest, ValuesDiffer) {
  // Sixteen equal draws of 32 bits have a chance of 2^-480.
  uint32_t first = LIBC_NAMESPACE::arc4random();
  bool differs = false;
  for (int i = 0; i < 16; ++i)
    differs = differs || LIBC_NAMESPACE::arc4random() != first;
  ASSERT_TRUE(differs);
}

TEST(LlvmLibcArc4randomTest, UniformStaysBelowTheBound) {
  ASSERT_EQ(LIBC_NAMESPACE::arc4random_uniform(0), uint32_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::arc4random_uniform(1), uint32_t(0));
  for (int i = 0; i < 1000; ++i) {
    ASSERT_LT(LIBC_NAMESPACE::arc4random_uniform(2), uint32_t(2));
    ASSERT_LT(LIBC_NAMESPACE::arc4random_uniform(10), uint32_t(10));
    ASSERT_LT(LIBC_NAMESPACE::arc4random_uniform(0x80000001u), 0x80000001u);
  }
}

TEST(LlvmLibcArc4randomTest, UniformReachesEveryValue) {
  // Each of 6 values is missed in 600 draws with a chance of about 2^-158.
  bool seen[6] = {};
  for (int i = 0; i < 600; ++i)
    seen[LIBC_NAMESPACE::arc4random_uniform(6)] = true;
  for (bool value : seen)
    ASSERT_TRUE(value);
}
