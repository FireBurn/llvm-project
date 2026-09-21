//===-- Unittests for gethostid ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for gethostid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/gethostid.h"
#include "test/UnitTest/Test.h"

// The identifier is read from /etc/hostid or derived from the host, and does
// not change from one call to the next.
TEST(LlvmLibcGethostidTest, IsStable) {
  long first = LIBC_NAMESPACE::gethostid();
  long second = LIBC_NAMESPACE::gethostid();
  ASSERT_EQ(first, second);
  // It is a 32 bit value, returned in a long.
  ASSERT_EQ(first, static_cast<long>(static_cast<int>(first)));
}
