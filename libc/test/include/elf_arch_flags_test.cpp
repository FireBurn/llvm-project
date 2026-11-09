//===-- Unittests for the ARM and RISC-V e_flags names in elf.h -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "hdr/elf_proxy.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcElfArchFlagsTest, ArmFlagsHavePsabiValues) {
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_EABI_VER5), 0x05000000u);
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_EABIMASK), 0xff000000u);
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_BE8), 0x00800000u);
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_ABI_FLOAT_HARD), 0x400u);
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_ABI_FLOAT_SOFT), 0x200u);
  EXPECT_EQ(static_cast<unsigned>(EF_ARM_EABI_VERSION(0x05000400u)),
            static_cast<unsigned>(EF_ARM_EABI_VER5));
}

TEST(LlvmLibcElfArchFlagsTest, RiscvFlagsHavePsabiValues) {
  EXPECT_EQ(static_cast<unsigned>(EF_RISCV_RVC), 1u);
  EXPECT_EQ(static_cast<unsigned>(EF_RISCV_FLOAT_ABI), 6u);
  EXPECT_EQ(static_cast<unsigned>(EF_RISCV_FLOAT_ABI_DOUBLE), 4u);
  EXPECT_EQ(static_cast<unsigned>(EF_RISCV_RVE), 8u);
  EXPECT_EQ(static_cast<unsigned>(EF_RISCV_TSO), 0x10u);
}

TEST(LlvmLibcElfArchFlagsTest, AndroidSymbolInfoNames) {
  EXPECT_EQ(static_cast<unsigned>(ELF_ST_INFO(STB_GLOBAL, STT_FUNC)),
            static_cast<unsigned>(ELF64_ST_INFO(STB_GLOBAL, STT_FUNC)));
  EXPECT_EQ(static_cast<unsigned>(ELF_ST_BIND(ELF_ST_INFO(STB_WEAK, STT_OBJECT))),
            static_cast<unsigned>(STB_WEAK));
  EXPECT_EQ(static_cast<unsigned>(ELF_ST_TYPE(ELF_ST_INFO(STB_WEAK, STT_OBJECT))),
            static_cast<unsigned>(STT_OBJECT));
}
