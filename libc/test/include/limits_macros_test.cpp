//===-- Unittests for the limits macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the limits macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/limits-macros.h"

TEST(LlvmLibcLimitsMacrosTest, AddNgroups_MaxAnd_Posix_Ngroups_Max) {
  EXPECT_EQ(static_cast<long long>(_POSIX_NGROUPS_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(NGROUPS_MAX), 0x10000LL);
}
