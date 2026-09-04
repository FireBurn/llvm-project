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

TEST(LlvmLibcElfMacrosTest, CompleteTheX86RelocationTypesAndAddThose) {
  EXPECT_EQ(static_cast<long long>(R_386_16), 20LL);
  EXPECT_EQ(static_cast<long long>(R_386_32PLT), 11LL);
  EXPECT_EQ(static_cast<long long>(R_386_8), 22LL);
  EXPECT_EQ(static_cast<long long>(R_386_COPY), 5LL);
  EXPECT_EQ(static_cast<long long>(R_386_GLOB_DAT), 6LL);
  EXPECT_EQ(static_cast<long long>(R_386_GOT32), 3LL);
  EXPECT_EQ(static_cast<long long>(R_386_GOT32X), 43LL);
  EXPECT_EQ(static_cast<long long>(R_386_GOTOFF), 9LL);
  EXPECT_EQ(static_cast<long long>(R_386_GOTPC), 10LL);
  EXPECT_EQ(static_cast<long long>(R_386_IRELATIVE), 42LL);
  EXPECT_EQ(static_cast<long long>(R_386_JMP_SLOT), 7LL);
  EXPECT_EQ(static_cast<long long>(R_386_NONE), 0LL);
  EXPECT_EQ(static_cast<long long>(R_386_NUM), 44LL);
  EXPECT_EQ(static_cast<long long>(R_386_PC16), 21LL);
  EXPECT_EQ(static_cast<long long>(R_386_PC32), 2LL);
  EXPECT_EQ(static_cast<long long>(R_386_PC8), 23LL);
  EXPECT_EQ(static_cast<long long>(R_386_PLT32), 4LL);
  EXPECT_EQ(static_cast<long long>(R_386_RELATIVE), 8LL);
  EXPECT_EQ(static_cast<long long>(R_386_SIZE32), 38LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_DESC), 41LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_DESC_CALL), 40LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_DTPMOD32), 35LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_DTPOFF32), 36LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GD), 18LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GD_32), 24LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GD_CALL), 26LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GD_POP), 27LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GD_PUSH), 25LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GOTDESC), 39LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_GOTIE), 16LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_IE), 15LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_IE_32), 33LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDM), 19LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDM_32), 28LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDM_CALL), 30LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDM_POP), 31LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDM_PUSH), 29LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LDO_32), 32LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LE), 17LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_LE_32), 34LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_TPOFF), 14LL);
  EXPECT_EQ(static_cast<long long>(R_386_TLS_TPOFF32), 37LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_16), 12LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_32), 10LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_32S), 11LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_64), 1LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_8), 14LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_COPY), 5LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_DTPMOD64), 16LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_DTPOFF32), 21LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_DTPOFF64), 17LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GLOB_DAT), 6LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOT32), 3LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOT64), 27LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTOFF64), 25LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPC32), 26LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPC32_TLSDESC), 34LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPC64), 29LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPCREL), 9LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPCREL64), 28LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPCRELX), 41LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTPLT64), 30LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_GOTTPOFF), 22LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_IRELATIVE), 37LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_JUMP_SLOT), 7LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_NUM), 43LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PC16), 13LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PC32), 2LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PC64), 24LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PC8), 15LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PLT32), 4LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_PLTOFF64), 31LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_RELATIVE), 8LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_RELATIVE64), 38LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_REX_GOTPCRELX), 42LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_SIZE32), 32LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_SIZE64), 33LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TLSDESC), 36LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TLSDESC_CALL), 35LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TLSGD), 19LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TLSLD), 20LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TPOFF32), 23LL);
  EXPECT_EQ(static_cast<long long>(R_X86_64_TPOFF64), 18LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_CALL), 28LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_JUMP24), 29LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_MOVT_ABS), 44LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_MOVW_ABS_NC), 43LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_PC24), 1LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_REL32), 3LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_CALL), 10LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_JUMP19), 51LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_JUMP24), 30LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_MOVT_ABS), 48LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_MOVW_ABS_NC), 47LL);
  EXPECT_EQ(static_cast<long long>(R_ARM_THM_PC22), 10LL);
  EXPECT_EQ(static_cast<long long>(R_AARCH64_CALL26), 283LL);
  EXPECT_EQ(static_cast<long long>(R_MIPS_26), 4LL);
  EXPECT_EQ(static_cast<long long>(R_MIPS_LO16), 6LL);
  EXPECT_EQ(static_cast<long long>(R_RISCV_SUB32), 39LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_ALIGN), 102LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_RELAX), 100LL);
  EXPECT_EQ(static_cast<long long>(R_LARCH_SUB32), 55LL);
  EXPECT_EQ(static_cast<long long>(STT_SPARC_REGISTER), 13LL);
  EXPECT_EQ(static_cast<long long>(R_390_32), 4LL);
  EXPECT_EQ(static_cast<long long>(R_68K_32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_ALPHA_REFLONG), 1LL);
  EXPECT_EQ(static_cast<long long>(R_IA64_SECREL32LSB), 101LL);
  EXPECT_EQ(static_cast<long long>(R_PARISC_DIR32), 1LL);
  EXPECT_EQ(static_cast<long long>(R_PPC_UADDR32), 24LL);
  EXPECT_EQ(static_cast<long long>(R_SPARC_32), 3LL);
  EXPECT_EQ(static_cast<long long>(R_SPARC_UA32), 23LL);
  EXPECT_EQ(static_cast<long long>(SHT_MIPS_DWARF), 0x7000001eLL);
}
