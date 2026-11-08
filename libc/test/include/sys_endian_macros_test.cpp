//===-- Unittests for the linux/sys/stat macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the macros of sys/endian.h.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-endian-macros.h"

TEST(LlvmLibcSysEndianMacrosTest, BigEndianConversions) {
  EXPECT_EQ(static_cast<unsigned>(betoh16(htobe16(0x1234))), 0x1234u);
  EXPECT_EQ(static_cast<unsigned>(betoh32(htobe32(0x12345678u))), 0x12345678u);
  EXPECT_EQ(
      static_cast<unsigned long long>(betoh64(htobe64(0x0102030405060708ull))),
      0x0102030405060708ull);
}

TEST(LlvmLibcSysEndianMacrosTest, LittleEndianConversions) {
  EXPECT_EQ(static_cast<unsigned>(letoh16(htole16(0x1234))), 0x1234u);
  EXPECT_EQ(static_cast<unsigned>(letoh32(htole32(0x12345678u))), 0x12345678u);
  EXPECT_EQ(
      static_cast<unsigned long long>(letoh64(htole64(0x0102030405060708ull))),
      0x0102030405060708ull);
}

TEST(LlvmLibcSysEndianMacrosTest, NetworkOrderIsBigEndian) {
  EXPECT_EQ(static_cast<unsigned long long>(htonq(0x0102030405060708ull)),
            static_cast<unsigned long long>(htobe64(0x0102030405060708ull)));
  EXPECT_EQ(
      static_cast<unsigned long long>(ntohq(htonq(0x0102030405060708ull))),
      0x0102030405060708ull);
}

TEST(LlvmLibcSysEndianMacrosTest, Swap) {
  EXPECT_EQ(static_cast<unsigned>(swap16(0x1234)), 0x3412u);
  EXPECT_EQ(static_cast<unsigned>(swap32(0x12345678u)), 0x78563412u);
  EXPECT_EQ(static_cast<unsigned long long>(swap64(0x0102030405060708ull)),
            0x0807060504030201ull);
}
