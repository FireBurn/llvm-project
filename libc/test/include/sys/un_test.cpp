//===-- Unittests for the sys/un.h macros ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-un-macros.h"
#include "include/llvm-libc-types/struct_sockaddr_un.h"

// The addresses are constants, so the lengths are worked out while compiling
// and the test needs no strlen of its own to link against.

TEST(LlvmLibcSysUnTest, SunLenCountsTheFamilyAndThePath) {
  static constexpr struct sockaddr_un ADDR = {0, "/run"};
  constexpr auto LENGTH = SUN_LEN(&ADDR);
  ASSERT_EQ(LENGTH, sizeof(ADDR.sun_family) + 4);
}

// An unnamed socket's address is the family and nothing else.
TEST(LlvmLibcSysUnTest, AnEmptyPathIsJustTheFamily) {
  static constexpr struct sockaddr_un ADDR = {0, ""};
  constexpr auto LENGTH = SUN_LEN(&ADDR);
  ASSERT_EQ(LENGTH, sizeof(ADDR.sun_family));
}
