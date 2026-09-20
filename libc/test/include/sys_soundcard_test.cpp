//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/soundcard.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/soundcard.h"
#include "test/UnitTest/Test.h"

#ifndef SNDCTL_DSP_RESET
#error "<sys/soundcard.h> does not carry the kernel's sound macros"
#endif

TEST(LlvmLibcSysSoundcardTest, TheKernelsMacrosAreThere) {
  EXPECT_NE(SNDCTL_DSP_RESET, 0u);
}
