//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android_mallopt.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/android/android_mallopt.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAndroidMalloptTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcAndroidMalloptTest, ZygoteCallsSucceedWithoutArguments) {
  EXPECT_TRUE(LIBC_NAMESPACE::android_mallopt(4, nullptr, 0));
  EXPECT_TRUE(LIBC_NAMESPACE::android_mallopt(1, nullptr, 0));
  int arg = 0;
  EXPECT_FALSE(LIBC_NAMESPACE::android_mallopt(4, &arg, sizeof(arg)));
}

TEST_F(LlvmLibcAndroidMalloptTest, QuestionsAboutTheHeapAreAnswered) {
  bool answer = true;
  EXPECT_TRUE(LIBC_NAMESPACE::android_mallopt(9, &answer, sizeof(answer)));
  EXPECT_FALSE(answer);
  EXPECT_TRUE(LIBC_NAMESPACE::android_mallopt(11, &answer, sizeof(answer)));
  EXPECT_TRUE(LIBC_NAMESPACE::android_mallopt(12, &answer, sizeof(answer)));
  EXPECT_FALSE(LIBC_NAMESPACE::android_mallopt(9, &answer, 1 + sizeof(answer)));
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcAndroidMalloptTest, UnsupportedOpcodesFail) {
  // M_INITIALIZE_GWP_ASAN
  EXPECT_FALSE(LIBC_NAMESPACE::android_mallopt(10, nullptr, 0));
  ASSERT_ERRNO_EQ(ENOTSUP);
  libc_errno = 0;
  EXPECT_FALSE(LIBC_NAMESPACE::android_mallopt(-1, nullptr, 0));
  ASSERT_ERRNO_EQ(ENOTSUP);
}
