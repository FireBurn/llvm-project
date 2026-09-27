//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/cdefs.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/cdefs.h"
#include "test/UnitTest/Test.h"

__BEGIN_DECLS
static int declared_in_a_c_block(void) { return 4; }
__END_DECLS

TEST(LlvmLibcSysCdefsTest, BeginAndEndDeclsBracketABlock) {
  EXPECT_EQ(declared_in_a_c_block(), 4);
}
