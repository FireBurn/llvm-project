//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the size_t pwd.h now brings in
///
//===----------------------------------------------------------------------===//

#include "include/pwd.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcPwdSizeTTest, SizeTComesWithPwd) {
  size_t length = sizeof(struct passwd);
  EXPECT_GT(length, size_t(0));
}
