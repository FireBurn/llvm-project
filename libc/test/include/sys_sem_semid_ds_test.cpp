//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for struct semid_ds
///
//===----------------------------------------------------------------------===//

#include "include/sys/sem.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysSemSemidDsTest, MembersAreTheKernelsOnX86_64) {
  struct semid_ds ds;
  __builtin_memset(&ds, 0, sizeof(ds));
  ds.sem_nsems = 4;
  EXPECT_EQ(static_cast<long long>(ds.sem_nsems), static_cast<long long>(4ul));
  ds.sem_otime = 7;
  EXPECT_EQ(static_cast<long long>(ds.sem_otime), static_cast<long long>(7));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(static_cast<long long>(sizeof(struct semid_ds)), static_cast<long long>(size_t(104)));
#endif
}
