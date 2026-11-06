//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests that no public type names a member __unused, which Android
/// defines as a macro.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/sys/cdefs.h"
#include "include/llvm-libc-types/struct_hsearch_data.h"
#include "include/llvm-libc-types/struct_icmphdr.h"
#include "include/llvm-libc-types/struct_stat.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidUnusedMacroTest, TypesStillDeclare) {
  EXPECT_EQ(sizeof(struct hsearch_data), sizeof(struct hsearch_data));
  EXPECT_EQ(sizeof(struct icmphdr), size_t(8));
  EXPECT_GE(sizeof(struct stat), size_t(100));
}
