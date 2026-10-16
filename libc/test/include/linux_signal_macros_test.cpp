//===-- Unittests for the linux/signal macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux signal macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/signal-macros.h"

TEST(LlvmLibcLinuxSignalMacrosTest, AddTheRemainingSi_CodeValues) {
  EXPECT_EQ(static_cast<long long>(ILL_BADIADDR), 9LL);
  EXPECT_EQ(static_cast<long long>(FPE_FLTUNK), 14LL);
  EXPECT_EQ(static_cast<long long>(FPE_CONDTRAP), 15LL);
  EXPECT_EQ(static_cast<long long>(SEGV_BNDERR), 3LL);
  EXPECT_EQ(static_cast<long long>(SEGV_PKUERR), 4LL);
  EXPECT_EQ(static_cast<long long>(SEGV_MTEAERR), 8LL);
  EXPECT_EQ(static_cast<long long>(SEGV_MTESERR), 9LL);
  EXPECT_EQ(static_cast<long long>(SEGV_CPERR), 10LL);
  EXPECT_EQ(static_cast<long long>(BUS_MCEERR_AR), 4LL);
  EXPECT_EQ(static_cast<long long>(BUS_MCEERR_AO), 5LL);
  EXPECT_EQ(static_cast<long long>(TRAP_BRANCH), 3LL);
  EXPECT_EQ(static_cast<long long>(TRAP_HWBKPT), 4LL);
  EXPECT_EQ(static_cast<long long>(TRAP_UNK), 5LL);
  EXPECT_EQ(static_cast<long long>(TRAP_PERF), 6LL);
}

TEST(LlvmLibcLinuxSignalMacrosTest, SigsysCodes) {
  EXPECT_EQ(static_cast<long long>(SYS_SECCOMP), 1LL);
  EXPECT_EQ(static_cast<long long>(SYS_USER_DISPATCH), 2LL);
}

TEST(LlvmLibcLinuxSignalMacrosTest, AddSegv_AccadiSegv_AdiderrAndSegv_Adiper) {
  EXPECT_EQ(static_cast<long long>(SEGV_ACCADI), 5LL);
  EXPECT_EQ(static_cast<long long>(SEGV_ADIDERR), 6LL);
  EXPECT_EQ(static_cast<long long>(SEGV_ADIPERR), 7LL);
}

TEST(LlvmLibcLinuxSignalMacrosTest, DefineSa_Nomask) {
  EXPECT_EQ(static_cast<long long>(SA_NOMASK), 0x40000000LL);
}

TEST(LlvmLibcLinuxSignalMacrosTest, DefineSigcld) {
  EXPECT_EQ(static_cast<long long>(SIGCLD), 17LL);
}
