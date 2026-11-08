//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that sys/eventfd.h gives Android programs the declarations of
/// fcntl.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-eventfd-macros.h"

TEST(LlvmLibcAndroidSysEventfdTest, FcntlHDeclarationsComeWithEventfdHeader) {
  // Naming open's return type is enough to need its declaration, and nothing
  // is linked against it.
  static_assert(sizeof(decltype(open("", 0))) == sizeof(int));
  EXPECT_EQ(EFD_NONBLOCK, O_NONBLOCK);
}
