//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the hardware capability names in the public sys/auxv.h.
///
//===----------------------------------------------------------------------===//

#include "include/sys/auxv.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysAuxvHwcapTest, NamesOfTheCapabilityBits) {
#if defined(__linux__) && defined(__aarch64__)
  EXPECT_EQ(HWCAP_ASIMD, 1 << 1);
  EXPECT_EQ(HWCAP2_MTE, 1 << 18);
#endif
  EXPECT_EQ(AT_HWCAP2, 26);
}
