//===-- Unittests for issetugid -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/issetugid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcIssetugidTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// A test runs as the user who started it and gains nothing on the way, so the
// kernel has no reason to mark it.
TEST_F(LlvmLibcIssetugidTest, AnOrdinaryProcessIsNotMarked) {
  ASSERT_EQ(LIBC_NAMESPACE::issetugid(), 0);
}
