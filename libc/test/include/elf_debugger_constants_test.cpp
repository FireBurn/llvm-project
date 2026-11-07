//===-- Unittests for the ARM and RISC-V constants in elf.h ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/elf_proxy.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcElfDebuggerConstantsTest, HaveTheirAbiValues) {
  EXPECT_EQ(static_cast<long long>(PT_ARM_EXIDX), 0x70000001LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_SVE), 0x405LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_VFP), 0x400LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_TLS), 0x401LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_HW_BREAK), 0x402LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_HW_WATCH), 0x403LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_SYSTEM_CALL), 0x404LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_PAC_MASK), 0x406LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_PACA_KEYS), 0x407LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_PACG_KEYS), 0x408LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_TAGGED_ADDR_CTRL), 0x409LL);
  EXPECT_EQ(static_cast<long long>(NT_ARM_PAC_ENABLED_KEYS), 0x40aLL);
  EXPECT_EQ(static_cast<long long>(NT_RISCV_VECTOR), 0x901LL);
}
