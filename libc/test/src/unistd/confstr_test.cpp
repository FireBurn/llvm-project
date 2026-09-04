//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for confstr
///
//===----------------------------------------------------------------------===//

#include "src/unistd/confstr.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/unistd_macros.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"

using LlvmLibcConfStrTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;

TEST_F(LlvmLibcConfStrTest, InvalidName) {
  char buf[64] = "initial";
  // _CS_PATH is zero, so a name has to be something else to be invalid.
  EXPECT_THAT(LIBC_NAMESPACE::confstr(-1, buf, sizeof(buf)),
              Fails(EINVAL, size_t(0)));
  EXPECT_THAT(LIBC_NAMESPACE::confstr(9999, buf, sizeof(buf)),
              Fails(EINVAL, size_t(0)));
  EXPECT_THAT(LIBC_NAMESPACE::confstr(9999, nullptr, 0),
              Fails(EINVAL, size_t(0)));
}

TEST_F(LlvmLibcConfStrTest, PathIsReported) {
  char buf[64] = "initial";
  // The full value plus its terminator, which is what a caller sizes a
  // buffer from.
  constexpr size_t EXPECTED = sizeof("/bin:/usr/bin");
  EXPECT_EQ(LIBC_NAMESPACE::confstr(_CS_PATH, buf, sizeof(buf)), EXPECTED);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_STREQ(buf, "/bin:/usr/bin");
}

TEST_F(LlvmLibcConfStrTest, SizeQueryLeavesTheBufferAlone) {
  char buf[64] = "initial";
  // A length of zero asks how much room is needed and must not write.
  EXPECT_EQ(LIBC_NAMESPACE::confstr(_CS_PATH, buf, 0), sizeof("/bin:/usr/bin"));
  EXPECT_STREQ(buf, "initial");
  EXPECT_EQ(LIBC_NAMESPACE::confstr(_CS_PATH, nullptr, 0),
            sizeof("/bin:/usr/bin"));
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcConfStrTest, ShortBufferIsTruncatedAndTerminated) {
  char buf[5];
  // The return value is what the whole value would need, not what fitted.
  EXPECT_EQ(LIBC_NAMESPACE::confstr(_CS_PATH, buf, sizeof(buf)),
            sizeof("/bin:/usr/bin"));
  EXPECT_STREQ(buf, "/bin");
}
