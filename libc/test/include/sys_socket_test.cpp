//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the types in sys/socket.h.
///
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-types/socklen_t.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysSocketTest, SocklenT) {
  static_assert(sizeof(socklen_t) == 4, "");
  static_assert(static_cast<socklen_t>(-1) > 0, "");
}
