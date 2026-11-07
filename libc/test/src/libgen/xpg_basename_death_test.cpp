//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Death tests for __xpg_basename.
///
//===----------------------------------------------------------------------===//

#include "src/libgen/xpg_basename.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcXpgBasenameTest, ModifyReturnValue) {
  char *r = LIBC_NAMESPACE::__xpg_basename(nullptr);
  ASSERT_DEATH([r]() { r[0] = 'a'; }, WITH_SIGNAL(-1));
}
