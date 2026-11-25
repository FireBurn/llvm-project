//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for arpa/nameser.h
///
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/arpa-nameser-macros.h"
#include "include/llvm-libc-types/HEADER.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "test/UnitTest/Test.h"

// The header is the twelve bytes a message starts with, and the members have
// to sit where the bytes carry them.
TEST(LlvmLibcArpaNameserTest, HeaderIsTwelveBytes) {
  ASSERT_EQ(sizeof(HEADER), size_t(12));
  ASSERT_EQ(size_t(NS_HFIXEDSZ), sizeof(HEADER));
}

// The two bytes of flags are written in the order the wire carries them,
// whichever way round the machine holds its integers.
TEST(LlvmLibcArpaNameserTest, FlagsLandOnTheWireBytes) {
  HEADER header;
  unsigned char raw[sizeof(HEADER)];
  for (size_t i = 0; i < sizeof(header); ++i)
    reinterpret_cast<unsigned char *>(&header)[i] = 0;

  // A reply to a plain question, asked with recursion, which found no name.
  header.qr = 1;
  header.opcode = 0;
  header.rd = 1;
  header.rcode = 3;
  LIBC_NAMESPACE::inline_memcpy(raw, &header, sizeof(raw));
  ASSERT_EQ(int(raw[2]), 0x81);
  ASSERT_EQ(int(raw[3]), 0x03);
}


TEST(LlvmLibcArpaNameserTest, MaxDomainHostName) {
  ASSERT_EQ(NS_MAXDHOSTNAME, 256);
}
