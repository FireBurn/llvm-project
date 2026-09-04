//===-- Unittests for getlogin and getlogin_r -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "src/unistd/getlogin.h"
#include "src/unistd/getlogin_r.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetLoginTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

TEST_F(LlvmLibcGetLoginTest, TakesTheNameFromTheEnvironment) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LOGNAME", "someone", 1), 0);

  char buf[64] = {};
  EXPECT_EQ(LIBC_NAMESPACE::getlogin_r(buf, sizeof(buf)), 0);
  EXPECT_TRUE(string_view(buf) == "someone");

  char *name = LIBC_NAMESPACE::getlogin();
  ASSERT_TRUE(name != nullptr);
  EXPECT_TRUE(string_view(name) == "someone");
}

TEST_F(LlvmLibcGetLoginTest, ABufferTooSmallIsReported) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LOGNAME", "a-rather-long-name", 1), 0);

  char buf[4] = {};
  // The name plus its terminator does not fit, which is ERANGE rather than a
  // truncated answer.
  EXPECT_EQ(LIBC_NAMESPACE::getlogin_r(buf, sizeof(buf)), ERANGE);

  // A buffer of exactly the right size does fit.
  char exact[19] = {};
  EXPECT_EQ(LIBC_NAMESPACE::getlogin_r(exact, sizeof(exact)), 0);
  EXPECT_TRUE(string_view(exact) == "a-rather-long-name");
}

TEST_F(LlvmLibcGetLoginTest, BadArgumentsAreReported) {
  char buf[8];
  EXPECT_EQ(LIBC_NAMESPACE::getlogin_r(nullptr, sizeof(buf)), EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::getlogin_r(buf, 0), EINVAL);
}

TEST_F(LlvmLibcGetLoginTest, FallsBackToThePasswordDatabase) {
  ASSERT_EQ(LIBC_NAMESPACE::unsetenv("LOGNAME"), 0);

  char buf[256] = {};
  int ret = LIBC_NAMESPACE::getlogin_r(buf, sizeof(buf));
  // With no LOGNAME the answer comes from the password database, which the
  // test may be running without an entry in.
  if (ret == 0) {
    EXPECT_FALSE(string_view(buf).empty());
  } else {
    EXPECT_EQ(ret, ENXIO);
  }
}

TEST_F(LlvmLibcGetLoginTest, AnEmptyLognameIsIgnored) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LOGNAME", "", 1), 0);

  char buf[256] = {};
  // An empty value is no answer at all, so the database is consulted.
  int ret = LIBC_NAMESPACE::getlogin_r(buf, sizeof(buf));
  if (ret == 0)
    EXPECT_FALSE(string_view(buf).empty());
  else
    EXPECT_EQ(ret, ENXIO);
}
