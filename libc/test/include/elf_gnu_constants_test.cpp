//===-- Unittests for the GNU and Solaris constants in elf.h --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/elf_proxy.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcElfGnuConstantsTest, HaveGlibcValues) {
  EXPECT_EQ(static_cast<unsigned long long>(SHN_BEFORE), 0xff00ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHN_AFTER), 0xff01ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_RELR), 19ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_ATTRIBUTES), 0x6ffffff5ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_HASH), 0x6ffffff6ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_LIBLIST), 0x6ffffff7ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_CHECKSUM), 0x6ffffff8ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_LOSUNW), 0x6ffffffaULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_SUNW_move), 0x6ffffffaULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_SUNW_COMDAT), 0x6ffffffbULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_SUNW_syminfo), 0x6ffffffcULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_verdef), 0x6ffffffdULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_verneed), 0x6ffffffeULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_GNU_versym), 0x6fffffffULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_HISUNW), 0x6fffffffULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHF_GNU_RETAIN), 0x200000ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHF_ORDERED), 0x40000000ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHF_EXCLUDE), 0x80000000ULL);
  EXPECT_EQ(static_cast<unsigned long long>(GRP_COMDAT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_BT_SELF), 0xffffULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_BT_PARENT), 0xfffeULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_BT_LOWRESERVE), 0xff00ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_FLG_DIRECT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_FLG_PASSTHRU), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_FLG_COPY), 4ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_FLG_LAZYLOAD), 8ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_NONE), 0ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_CURRENT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SYMINFO_NUM), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(PT_GNU_SFRAME), 0x6474e554ULL);
  EXPECT_EQ(static_cast<unsigned long long>(PT_LOSUNW), 0x6ffffffaULL);
  EXPECT_EQ(static_cast<unsigned long long>(PT_SUNWBSS), 0x6ffffffaULL);
  EXPECT_EQ(static_cast<unsigned long long>(PT_SUNWSTACK), 0x6ffffffbULL);
  EXPECT_EQ(static_cast<unsigned long long>(PT_HISUNW), 0x6fffffffULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_GWINDOWS), 7ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_ASRS), 8ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_PSTATUS), 10ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_PSINFO), 13ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_PRCRED), 14ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_UTSNAME), 15ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LWPSTATUS), 16ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LWPSINFO), 17ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_PRXFPREG), 0x46e62b7fULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_386_TLS), 0x200ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_386_IOPERM), 0x201ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_X86_XSTATE), 0x202ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_X86_SHSTK), 0x204ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_X86_XSAVE_LAYOUT), 0x205ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_VMCOREDD), 0x700ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_CPUCFG), 0xa00ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_CSR), 0xa01ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_LSX), 0xa02ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_LASX), 0xa03ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_LBT), 0xa04ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_HW_BREAK), 0xa05ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_LOONGARCH_HW_WATCH), 0xa06ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_VERSION), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(DTF_1_PARINIT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(DTF_1_CONFEXP), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(DF_P1_LAZYLOAD), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(DF_P1_GROUPPERM), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(VER_NEED_NONE), 0ULL);
  EXPECT_EQ(static_cast<unsigned long long>(VER_NEED_CURRENT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(ELF_NOTE_PAGESIZE_HINT), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(ELF_NOTE_OS_LINUX), 0ULL);
  EXPECT_EQ(static_cast<unsigned long long>(ELF_NOTE_OS_GNU), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(ELF_NOTE_OS_SOLARIS2), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(ELF_NOTE_OS_FREEBSD), 3ULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_FDO_PACKAGING_METADATA),
            0xcafe1a7eULL);
  EXPECT_EQ(static_cast<unsigned long long>(NT_FDO_DLOPEN_METADATA),
            0x407c0c0aULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_NONE), 0ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_EXACT_MATCH), 1ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_IGNORE_INT_VER), 2ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_REQUIRE_MINOR), 4ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_EXPORTS), 8ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_DELAY_LOAD), 16ULL);
  EXPECT_EQ(static_cast<unsigned long long>(LL_DELTA), 32ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_CSKY_ATTRIBUTES),
            0x70000001ULL);
  EXPECT_EQ(static_cast<unsigned long long>(DT_NIOS2_GP), 0x70000002ULL);
  EXPECT_EQ(static_cast<unsigned long long>(SHT_ARC_ATTRIBUTES), 0x70000001ULL);
}
