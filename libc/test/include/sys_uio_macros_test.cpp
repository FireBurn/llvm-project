//===-- Unittests for the sys/uio macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the sys uio macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-uio-macros.h"

TEST(LlvmLibcSysUioMacrosTest, DefineUio_Maxiov) {
  EXPECT_EQ(static_cast<long long>(UIO_MAXIOV), 1024LL);
}
