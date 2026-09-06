//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Tests for res_init.
///
//===----------------------------------------------------------------------===//

#include "src/compat/res_init.h"
#include "test/UnitTest/Test.h"

// Nothing is held between lookups, so asking for the configuration to be read
// again has nothing to do and succeeds, under both names.
TEST(LlvmLibcResInitTest, SucceedsUnderBothNames) {
  ASSERT_EQ(LIBC_NAMESPACE::res_init(), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__res_init(), 0);
  // And again, since a caller may do it after every failed lookup.
  ASSERT_EQ(LIBC_NAMESPACE::res_init(), 0);
}
