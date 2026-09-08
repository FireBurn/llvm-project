//===-- Unittests for the barrier types ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the barrier types.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/__barrier_type.h"
#include "include/llvm-libc-types/pthread_barrierattr_t.h"

// Before C23 <stdbool.h> defines bool as a macro, which a public header may
// not leave behind.
#ifdef bool
#error "the barrier types define bool"
#endif
#ifdef true
#error "the barrier types define true"
#endif

TEST(LlvmLibcBarrierTypeTest, IsUsable) {
  __barrier_type barrier = {};
  ASSERT_EQ(alignof(__barrier_type), size_t(8));
  ASSERT_GE(sizeof(barrier), size_t(8));
}
