//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for pty.h bringing in termios.h, sys/ioctl.h
///
//===----------------------------------------------------------------------===//

#include "include/pty.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_TERMIOS_H
#error "<pty.h> does not bring in <termios.h>"
#endif
#ifndef _LLVM_LIBC_SYS_IOCTL_H
#error "<pty.h> does not bring in <sys/ioctl.h>"
#endif

TEST(LlvmLibcPtyTermiosTest, CarriesTheHeaders) {
  struct termios attributes;
  struct winsize size;
  EXPECT_GT(sizeof(attributes), size_t(0));
  EXPECT_GT(sizeof(size), size_t(0));
}
