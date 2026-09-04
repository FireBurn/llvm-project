//===-- Unittests for the elf macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the elf macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/elf-macros.h"

TEST(LlvmLibcElfMacrosTest, AddTheElfMachineTypesAndAbsoluteRelocati) {
  EXPECT_EQ(static_cast<long long>(R_386_NONE), 0LL);
  EXPECT_EQ(static_cast<long long>(R_386_32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_NONE), 0LL);
  EXPECT_EQ(static_cast<long long>(R_AARCH64_NONE), 0LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_NONE), 0LL);
  EXPECT_EQ(static_cast<long long>(R_MIPS_32), 2LL);
  EXPECT_EQ(static_cast<long long>(R_MIPS_64), 18LL);
  EXPECT_EQ(static_cast<long long>(R_PPC_ADDR32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_PPC64_ADDR64), 38LL);
  EXPECT_EQ(static_cast<long long>(R_390_64), 22LL);
  EXPECT_EQ(static_cast<long long>(R_SH_DIR32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_SPARC_64), 32LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_64), 2LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_MARK_LA), 20LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_SOP_PUSH_PLT_PCREL), 29LL);
}
