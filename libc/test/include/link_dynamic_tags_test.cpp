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

TEST(LlvmLibcLinkDynamicTagsTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"DT_ADDRNUM", static_cast<long long>(DT_ADDRNUM), 11LL},
      {"DT_AUXILIARY", static_cast<long long>(DT_AUXILIARY), 0x7ffffffdLL},
      {"DT_CHECKSUM", static_cast<long long>(DT_CHECKSUM), 0x6ffffdf8LL},
      {"DT_EXTRANUM", static_cast<long long>(DT_EXTRANUM), 3LL},
      {"DT_FEATURE_1", static_cast<long long>(DT_FEATURE_1), 0x6ffffdfcLL},
      {"DT_FILTER", static_cast<long long>(DT_FILTER), 0x7fffffffLL},
      {"DT_FLAGS_1", static_cast<long long>(DT_FLAGS_1), 0x6ffffffbLL},
      {"DT_GNU_CONFLICTSZ", static_cast<long long>(DT_GNU_CONFLICTSZ), 0x6ffffdf6LL},
      {"DT_GNU_LIBLISTSZ", static_cast<long long>(DT_GNU_LIBLISTSZ), 0x6ffffdf7LL},
      {"DT_GNU_PRELINKED", static_cast<long long>(DT_GNU_PRELINKED), 0x6ffffdf5LL},
      {"DT_MOVEENT", static_cast<long long>(DT_MOVEENT), 0x6ffffdfaLL},
      {"DT_MOVESZ", static_cast<long long>(DT_MOVESZ), 0x6ffffdfbLL},
      {"DT_PLTPADSZ", static_cast<long long>(DT_PLTPADSZ), 0x6ffffdf9LL},
      {"DT_POSFLAG_1", static_cast<long long>(DT_POSFLAG_1), 0x6ffffdfdLL},
      {"DT_RELR", static_cast<long long>(DT_RELR), 36LL},
      {"DT_RELRENT", static_cast<long long>(DT_RELRENT), 37LL},
      {"DT_RELRSZ", static_cast<long long>(DT_RELRSZ), 35LL},
      {"DT_SYMINENT", static_cast<long long>(DT_SYMINENT), 0x6ffffdffLL},
      {"DT_SYMINSZ", static_cast<long long>(DT_SYMINSZ), 0x6ffffdfeLL},
      {"DT_VALNUM", static_cast<long long>(DT_VALNUM), 12LL},
      {"DT_VALRNGHI", static_cast<long long>(DT_VALRNGHI), 0x6ffffdffLL},
      {"DT_VALRNGLO", static_cast<long long>(DT_VALRNGLO), 0x6ffffd00LL},
      {"DT_VERSIONTAGNUM", static_cast<long long>(DT_VERSIONTAGNUM), 16LL},
      {"DT_X86_64_NUM", static_cast<long long>(DT_X86_64_NUM), 4LL},
      {"DT_X86_64_PLT", static_cast<long long>(DT_X86_64_PLT), 0x70000000LL},
      {"DT_X86_64_PLTENT", static_cast<long long>(DT_X86_64_PLTENT), 0x70000003LL},
      {"DT_X86_64_PLTSZ", static_cast<long long>(DT_X86_64_PLTSZ), 0x70000001LL},
      {"DT_MIPS_RLD_MAP_REL", static_cast<long long>(DT_MIPS_RLD_MAP_REL), 0x70000035LL},
      {"DT_MIPS_RLD_MAP", static_cast<long long>(DT_MIPS_RLD_MAP), 0x70000016LL},
      {"DT_MIPS_NUM", static_cast<long long>(DT_MIPS_NUM), 55LL},
      {"DT_AARCH64_BTI_PLT", static_cast<long long>(DT_AARCH64_BTI_PLT), 0x70000001LL},
      {"DT_AARCH64_PAC_PLT", static_cast<long long>(DT_AARCH64_PAC_PLT), 0x70000003LL},
      {"DT_AARCH64_VARIANT_PCS", static_cast<long long>(DT_AARCH64_VARIANT_PCS), 0x70000005LL},
      {"DT_AARCH64_NUM", static_cast<long long>(DT_AARCH64_NUM), 6LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}
