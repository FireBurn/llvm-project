//===-- Unittests for acosh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/acosh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAcoshTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcAcoshTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::acosh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::acosh(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::acosh(1.0));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::acosh(inf));
}

// acosh is only defined from one upwards.
TEST_F(LlvmLibcAcoshTest, OutOfDomain) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_IS_NAN_WITH_EXCEPTION(LIBC_NAMESPACE::acosh(0.0), FE_INVALID);
  EXPECT_MATH_ERRNO(EDOM);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_IS_NAN_WITH_EXCEPTION(LIBC_NAMESPACE::acosh(0.5), FE_INVALID);
  EXPECT_MATH_ERRNO(EDOM);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_IS_NAN_WITH_EXCEPTION(LIBC_NAMESPACE::acosh(neg_inf), FE_INVALID);
  EXPECT_MATH_ERRNO(EDOM);
}
