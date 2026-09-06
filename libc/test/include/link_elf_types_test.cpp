//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public link.h header.
///
//===----------------------------------------------------------------------===//

#include "include/link.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcLinkElfTypesTest, TypesAreComplete) {
  EXPECT_GT(sizeof(Elf32_Section), size_t(0));
  EXPECT_GT(sizeof(Elf32_Move), size_t(0));
  EXPECT_GT(sizeof(Elf32_Lib), size_t(0));
  EXPECT_GT(sizeof(Elf32_Syminfo), size_t(0));
  EXPECT_GT(sizeof(Elf64_Section), size_t(0));
  EXPECT_GT(sizeof(Elf64_Move), size_t(0));
  EXPECT_GT(sizeof(Elf64_Lib), size_t(0));
  EXPECT_GT(sizeof(Elf64_Syminfo), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(Elf32_Section), size_t(2));
  EXPECT_EQ(sizeof(Elf32_Move), size_t(24));
  EXPECT_EQ(sizeof(Elf32_Lib), size_t(20));
  EXPECT_EQ(sizeof(Elf32_Syminfo), size_t(4));
  EXPECT_EQ(sizeof(Elf64_Section), size_t(2));
  EXPECT_EQ(sizeof(Elf64_Move), size_t(32));
  EXPECT_EQ(sizeof(Elf64_Lib), size_t(20));
  EXPECT_EQ(sizeof(Elf64_Syminfo), size_t(4));
#endif
}
