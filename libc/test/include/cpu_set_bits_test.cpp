//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests the names of the word a cpu_set_t is made of.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/cpu_set_t.h"

TEST(LlvmLibcCpuSetBitsTest, BitTypeIsTheWordTheSetIsMadeOf) {
  EXPECT_EQ(sizeof(__CPU_BITTYPE), sizeof(__cpu_set_mask_t));
  EXPECT_EQ(static_cast<size_t>(__CPU_BITS), static_cast<size_t>(__NCPUBITS));
  EXPECT_EQ(sizeof(cpu_set_t) / sizeof(__CPU_BITTYPE),
            static_cast<size_t>(__CPU_SETSIZE) / __CPU_BITS);
  // The array goes by the name glibc, musl and bionic give it.
  cpu_set_t set = {};
  EXPECT_EQ(sizeof(set.__bits), sizeof(set));
}
