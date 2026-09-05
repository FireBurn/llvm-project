//===-- Unittests for h_errno ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/__h_errno_location.h"
#include "test/UnitTest/Test.h"

// h_errno is one place per thread, so every call hands back the same one,
// and what is written there stays until it is written again.
TEST(LlvmLibcHErrnoTest, IsOnePlaceThatKeepsWhatIsWritten) {
  int *place = LIBC_NAMESPACE::__h_errno_location();
  ASSERT_FALSE(place == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::__h_errno_location(), place);

  *place = 3;
  ASSERT_EQ(*LIBC_NAMESPACE::__h_errno_location(), 3);
  *place = 0;
  ASSERT_EQ(*LIBC_NAMESPACE::__h_errno_location(), 0);
}
