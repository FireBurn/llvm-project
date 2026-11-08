//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the statx types leave the kernel's definitions in place on
/// Android, where programs include both sys/stat.h and linux/stat.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/struct_statx.h"

// Without the guard in the header above, this is a redefinition.
#include <linux/stat.h>

#include <stddef.h>

TEST(LlvmLibcAndroidStatxTest, KernelHeaderCanBeIncludedAfterOurs) {
  EXPECT_EQ(sizeof(struct statx), size_t(256));
  EXPECT_EQ(sizeof(struct statx_timestamp), size_t(16));
}
