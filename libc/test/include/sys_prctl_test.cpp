//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that sys/prctl.h leaves arch_prctl to the program, as glibc and musl
/// do, so that a program defining its own does not clash with the header.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/sys/prctl.h"

// Not a declaration of ours: this definition must be accepted after the
// header, as wine's is.
static inline int arch_prctl(int, void *) { return 0; }

TEST(LlvmLibcSysPrctlHeaderTest, ProgramMayDefineArchPrctl) {
  ASSERT_EQ(arch_prctl(0, nullptr), 0);
}
