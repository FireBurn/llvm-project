//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __sched_cpucount and __fpclassifyf
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "hdr/types/cpu_set_t.h"
#include "src/compat/__fpclassifyf.h"
#include "src/compat/__sched_cpucount.h"
#include "src/string/memset.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcCompatTest, CountsTheCpusInASet) {
  cpu_set_t set;
  LIBC_NAMESPACE::memset(&set, 0, sizeof(set));
  ASSERT_EQ(LIBC_NAMESPACE::__sched_cpucount(sizeof(set), &set), 0);
  set.__bits[0] = 0x9;
  set.__bits[1] = 0x1;
  ASSERT_EQ(LIBC_NAMESPACE::__sched_cpucount(sizeof(set), &set), 3);
}

TEST(LlvmLibcCompatTest, ClassifiesAFloat) {
  ASSERT_EQ(LIBC_NAMESPACE::__fpclassifyf(0.0f), FP_ZERO);
  ASSERT_EQ(LIBC_NAMESPACE::__fpclassifyf(1.5f), FP_NORMAL);
  ASSERT_EQ(LIBC_NAMESPACE::__fpclassifyf(1e-45f), FP_SUBNORMAL);
  ASSERT_EQ(LIBC_NAMESPACE::__fpclassifyf(__builtin_inff()), FP_INFINITE);
  ASSERT_EQ(LIBC_NAMESPACE::__fpclassifyf(__builtin_nanf("")), FP_NAN);
}
