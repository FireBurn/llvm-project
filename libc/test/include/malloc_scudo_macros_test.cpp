//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests the mallopt parameters of Scudo that malloc.h names.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/malloc-macros.h"

TEST(LlvmLibcMallocScudoMacrosTest, ParametersHaveScudosNumbers) {
  EXPECT_EQ(M_DECAY_TIME, -100);
  EXPECT_EQ(M_MEMTAG_TUNING, -102);
  EXPECT_EQ(M_THREAD_DISABLE_MEM_INIT, -103);
  EXPECT_EQ(M_PURGE_FAST, -105);
  EXPECT_EQ(M_CACHE_COUNT_MAX, -200);
  EXPECT_EQ(M_CACHE_SIZE_MAX, -201);
  EXPECT_EQ(M_TSDS_COUNT_MAX, -202);
  EXPECT_EQ(M_LOG_STATS, -205);
  EXPECT_EQ(M_CACHE_RESIDENT_BYTES_MAX, -206);
  EXPECT_EQ(M_MEMTAG_TUNING_BUFFER_OVERFLOW, 0);
  EXPECT_EQ(M_MEMTAG_TUNING_UAF, 1);
}
