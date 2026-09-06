//===-- Unittests for the sys/mman macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the sys mman macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-mman-macros.h"

TEST(LlvmLibcSysMmanMacrosTest, AddTheBsdNamesForAnAnonymousMapping) {
  EXPECT_EQ(static_cast<long long>(MAP_FILE), 0LL);
}
