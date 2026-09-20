//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/procfs.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/procfs.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysProcfsTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct elf_siginfo), size_t(0));
  EXPECT_GT(sizeof(elf_gregset_t), size_t(0));
  EXPECT_GT(sizeof(elf_fpregset_t), size_t(0));
  EXPECT_GT(sizeof(struct elf_prstatus), size_t(0));
  EXPECT_GT(sizeof(struct elf_prpsinfo), size_t(0));
  EXPECT_GT(sizeof(prstatus_t), size_t(0));
  EXPECT_GT(sizeof(prpsinfo_t), size_t(0));
  EXPECT_GT(sizeof(prgregset_t), size_t(0));
  EXPECT_GT(sizeof(prfpregset_t), size_t(0));
  EXPECT_GT(sizeof(psaddr_t), size_t(0));
  EXPECT_GT(sizeof(lwpid_t), size_t(0));
  EXPECT_GT(sizeof(pid_t), size_t(0));
  EXPECT_GT(sizeof(struct timeval), size_t(0));
  EXPECT_GT(sizeof(struct user_fpregs_struct), size_t(0));
  EXPECT_GT(sizeof(struct user_regs_struct), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct elf_siginfo), size_t(12));
  EXPECT_EQ(sizeof(elf_gregset_t), size_t(216));
  EXPECT_EQ(sizeof(elf_fpregset_t), size_t(512));
  EXPECT_EQ(sizeof(struct elf_prstatus), size_t(336));
  EXPECT_EQ(sizeof(struct elf_prpsinfo), size_t(136));
  EXPECT_EQ(sizeof(prstatus_t), size_t(336));
  EXPECT_EQ(sizeof(prpsinfo_t), size_t(136));
  EXPECT_EQ(sizeof(prgregset_t), size_t(216));
  EXPECT_EQ(sizeof(prfpregset_t), size_t(512));
  EXPECT_EQ(sizeof(psaddr_t), size_t(8));
  EXPECT_EQ(sizeof(lwpid_t), size_t(4));
  EXPECT_EQ(sizeof(pid_t), size_t(4));
  EXPECT_EQ(sizeof(struct timeval), size_t(16));
  EXPECT_EQ(sizeof(struct user_fpregs_struct), size_t(512));
  EXPECT_EQ(sizeof(struct user_regs_struct), size_t(216));
#endif
}
