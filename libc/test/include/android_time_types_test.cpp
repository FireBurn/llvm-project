//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that Android programs can include the time types together with
/// <linux/time.h>, which defines them as well.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/llvm-libc-types/struct_itimerspec.h"
#include "include/llvm-libc-types/struct_itimerval.h"
#include "include/llvm-libc-types/struct_timespec.h"
#include "include/llvm-libc-types/struct_timeval.h"
#include "include/llvm-libc-types/struct_timezone.h"
#include "test/UnitTest/Test.h"

#include <linux/time.h>
#include <stddef.h>

TEST(LlvmLibcAndroidTimeTypesTest, LayoutsAreTheKernels) {
  EXPECT_EQ(sizeof(struct timespec), 2 * sizeof(long));
  EXPECT_EQ(sizeof(struct timeval), 2 * sizeof(long));
  EXPECT_EQ(sizeof(struct itimerspec), sizeof(struct timespec) * 2);
  EXPECT_EQ(sizeof(struct itimerval), sizeof(struct timeval) * 2);
  EXPECT_EQ(sizeof(struct timezone), 2 * sizeof(int));
  EXPECT_EQ(offsetof(struct timespec, tv_nsec), sizeof(long));
  EXPECT_EQ(offsetof(struct timeval, tv_usec), sizeof(long));
}
