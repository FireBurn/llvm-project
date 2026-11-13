//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public bits/ headers Android's headers include by name.
///
//===----------------------------------------------------------------------===//

#include "include/bits/sa_family_t.h"
#include "include/bits/sockaddr_storage.h"
#include "include/bits/timespec.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

TEST(LlvmLibcBitsHeadersTest, TimespecIsDefinedByBitsTimespec) {
  struct timespec ts = {1, 2};
  EXPECT_EQ(static_cast<long>(ts.tv_sec), 1L);
  EXPECT_EQ(static_cast<long>(ts.tv_nsec), 2L);
}

TEST(LlvmLibcBitsHeadersTest, SaFamilyIsAnUnsignedShort) {
  EXPECT_EQ(sizeof(sa_family_t), sizeof(unsigned short));
}

TEST(LlvmLibcBitsHeadersTest, SockaddrStorageHoldsAFamily) {
  struct sockaddr_storage ss = {};
  ss.ss_family = 7;
  EXPECT_EQ(static_cast<int>(ss.ss_family), 7);
  EXPECT_GE(sizeof(ss), size_t(128));
}
