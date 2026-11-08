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
/// Tests that elf.h gives Android programs bionic's IS_ELF.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/elf.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidElfTest, IsElfLooksAtTheMagicNumber) {
  Elf64_Ehdr header = {};
  EXPECT_FALSE(IS_ELF(header));
  header.e_ident[EI_MAG0] = ELFMAG0;
  header.e_ident[EI_MAG1] = ELFMAG1;
  header.e_ident[EI_MAG2] = ELFMAG2;
  header.e_ident[EI_MAG3] = ELFMAG3;
  EXPECT_TRUE(IS_ELF(header));
  header.e_ident[EI_MAG3] = 'X';
  EXPECT_FALSE(IS_ELF(header));

  Elf32_Ehdr header32 = {};
  header32.e_ident[EI_MAG0] = ELFMAG0;
  header32.e_ident[EI_MAG1] = ELFMAG1;
  header32.e_ident[EI_MAG2] = ELFMAG2;
  header32.e_ident[EI_MAG3] = ELFMAG3;
  EXPECT_TRUE(IS_ELF(header32));
}
