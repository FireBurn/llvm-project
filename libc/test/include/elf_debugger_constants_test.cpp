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
  EXPECT_EQ(static_cast<long long>(NT_RISCV_VECTOR), 0x901LL);
}
