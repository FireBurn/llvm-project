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

TEST(LlvmLibcLinkDtFlags1BitsTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"EM_FAKE_ALPHA", static_cast<long long>(EM_FAKE_ALPHA), 41LL},
      {"EM_ALPHA", static_cast<long long>(EM_ALPHA), 0x9026LL},
      {"DF_1_NOW", static_cast<long long>(DF_1_NOW), 1LL},
      {"DF_1_GLOBAL", static_cast<long long>(DF_1_GLOBAL), 2LL},
      {"DF_1_GROUP", static_cast<long long>(DF_1_GROUP), 4LL},
      {"DF_1_NODELETE", static_cast<long long>(DF_1_NODELETE), 8LL},
      {"DF_1_LOADFLTR", static_cast<long long>(DF_1_LOADFLTR), 16LL},
      {"DF_1_INITFIRST", static_cast<long long>(DF_1_INITFIRST), 32LL},
      {"DF_1_NOOPEN", static_cast<long long>(DF_1_NOOPEN), 64LL},
      {"DF_1_ORIGIN", static_cast<long long>(DF_1_ORIGIN), 128LL},
      {"DF_1_DIRECT", static_cast<long long>(DF_1_DIRECT), 256LL},
      {"DF_1_TRANS", static_cast<long long>(DF_1_TRANS), 512LL},
      {"DF_1_INTERPOSE", static_cast<long long>(DF_1_INTERPOSE), 1024LL},
      {"DF_1_NODEFLIB", static_cast<long long>(DF_1_NODEFLIB), 2048LL},
      {"DF_1_NODUMP", static_cast<long long>(DF_1_NODUMP), 0x1000LL},
      {"DF_1_CONFALT", static_cast<long long>(DF_1_CONFALT), 0x2000LL},
      {"DF_1_ENDFILTEE", static_cast<long long>(DF_1_ENDFILTEE), 0x4000LL},
      {"DF_1_DISPRELDNE", static_cast<long long>(DF_1_DISPRELDNE), 0x8000LL},
      {"DF_1_DISPRELPND", static_cast<long long>(DF_1_DISPRELPND), 0x10000LL},
      {"DF_1_NODIRECT", static_cast<long long>(DF_1_NODIRECT), 0x20000LL},
      {"DF_1_IGNMULDEF", static_cast<long long>(DF_1_IGNMULDEF), 0x40000LL},
      {"DF_1_NOKSYMS", static_cast<long long>(DF_1_NOKSYMS), 0x80000LL},
      {"DF_1_NOHDR", static_cast<long long>(DF_1_NOHDR), 0x100000LL},
      {"DF_1_EDITED", static_cast<long long>(DF_1_EDITED), 0x200000LL},
      {"DF_1_NORELOC", static_cast<long long>(DF_1_NORELOC), 0x400000LL},
      {"DF_1_SYMINTPOSE", static_cast<long long>(DF_1_SYMINTPOSE), 0x800000LL},
      {"DF_1_GLOBAUDIT", static_cast<long long>(DF_1_GLOBAUDIT), 0x1000000LL},
      {"DF_1_SINGLETON", static_cast<long long>(DF_1_SINGLETON), 0x2000000LL},
      {"DF_1_STUB", static_cast<long long>(DF_1_STUB), 0x4000000LL},
      {"DF_1_PIE", static_cast<long long>(DF_1_PIE), 0x8000000LL},
      {"DF_1_KMOD", static_cast<long long>(DF_1_KMOD), 0x10000000LL},
      {"DF_1_WEAKFILTER", static_cast<long long>(DF_1_WEAKFILTER), 0x20000000LL},
      {"DF_1_NOCOMMON", static_cast<long long>(DF_1_NOCOMMON), 0x40000000LL},
      {"PT_MIPS_REGINFO", static_cast<long long>(PT_MIPS_REGINFO), 0x70000000LL},
      {"PT_MIPS_RTPROC", static_cast<long long>(PT_MIPS_RTPROC), 0x70000001LL},
      {"PT_MIPS_OPTIONS", static_cast<long long>(PT_MIPS_OPTIONS), 0x70000002LL},
      {"PT_MIPS_ABIFLAGS", static_cast<long long>(PT_MIPS_ABIFLAGS), 0x70000003LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}
