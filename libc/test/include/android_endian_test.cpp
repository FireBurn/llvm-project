//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that endian.h gives Android programs the declarations of
/// sys/endian.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/endian.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidEndianTest, ByteSwapNamesComeWithEndianHeader) {
  EXPECT_EQ(static_cast<int>(swap16(0x0102)), 0x0201);
  EXPECT_EQ(static_cast<unsigned>(htobe32(1)), 0x01000000u);
}
