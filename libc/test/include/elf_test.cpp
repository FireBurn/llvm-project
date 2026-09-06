//===-- Unittests for <elf.h> macros --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/elf_proxy.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcElfTest, RelocationsTakeApartWhatTheyPutTogether) {
  // The symbol index of a 64 bit relocation lives above the type, and a
  // caller holding it in something 32 bits wide, which is where it comes
  // from, must still get it into the right half.
  unsigned int symbol = 0x1234;
  unsigned int type = 7;
  Elf64_Xword info = ELF64_R_INFO(symbol, type);
  EXPECT_EQ(info, Elf64_Xword(0x123400000007));
  EXPECT_EQ(Elf64_Xword(ELF64_R_SYM(info)), Elf64_Xword(symbol));
  EXPECT_EQ(Elf64_Xword(ELF64_R_TYPE(info)), Elf64_Xword(type));

  unsigned int symbol32 = 0x1234;
  unsigned int type32 = 0x42;
  Elf32_Word info32 = ELF32_R_INFO(symbol32, type32);
  EXPECT_EQ(info32, Elf32_Word(0x123442));
  EXPECT_EQ(Elf32_Word(ELF32_R_SYM(info32)), Elf32_Word(symbol32));
  EXPECT_EQ(Elf32_Word(ELF32_R_TYPE(info32)), Elf32_Word(type32));
}

TEST(LlvmLibcElfTest, TheRecordsAreTheSizesTheyAreOnTheWire) {
  EXPECT_EQ(sizeof(Elf64_Section), size_t(2));
  EXPECT_EQ(sizeof(Elf32_Section), size_t(2));
  EXPECT_EQ(sizeof(Elf64_Syminfo), size_t(4));
  EXPECT_EQ(sizeof(Elf64_Lib), size_t(20));
  EXPECT_EQ(sizeof(Elf64_Move), size_t(32));
  EXPECT_EQ(sizeof(Elf32_Move), size_t(24));
}
