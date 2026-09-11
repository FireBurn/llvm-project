//===-- Unittests for the sys/select.h macros and types -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-select-macros.h"
#include "include/llvm-libc-types/fd_mask.h"
#include "include/llvm-libc-types/fd_set.h"

TEST(LlvmLibcSysSelectTest, AWordHoldsNfdbitsDescriptors) {
  ASSERT_EQ(NFDBITS, sizeof(fd_mask) * 8);
  ASSERT_EQ(sizeof(fd_set), sizeof(fd_mask) * (FD_SETSIZE / NFDBITS));
}

// Code which walks a set a word at a time reads fds_bits, so the words there
// have to be the ones the FD_ macros change.
TEST(LlvmLibcSysSelectTest, TheMacrosChangeTheWordsCodeReads) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(3, &set);
  FD_SET(NFDBITS + 1, &set);

  fd_mask *words = set.fds_bits;
  ASSERT_EQ(words[0], fd_mask(1) << 3);
  ASSERT_EQ(words[1], fd_mask(1) << 1);
  ASSERT_EQ(FD_ISSET(3, &set), 1);
  ASSERT_EQ(FD_ISSET(4, &set), 0);

  FD_CLR(3, &set);
  ASSERT_EQ(words[0], fd_mask(0));
}
