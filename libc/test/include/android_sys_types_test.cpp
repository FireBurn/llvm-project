//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Tests that sys/types.h gives Android programs bionic's uint_t.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/sys/types.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidSysTypesTest, UintTIsUnsignedInt) {
  uint_t value = 0;
  --value;
  EXPECT_EQ(value, ~0u);
  EXPECT_EQ(sizeof(uint_t), sizeof(unsigned int));
}
