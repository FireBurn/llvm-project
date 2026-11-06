//===-- Unittests for the Android availability macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the availability macros sys/cdefs.h defines on Android.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/sys/cdefs.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

__BEGIN_DECLS
int introduced_late(void) __INTRODUCED_IN(35);
int introduced_late(void) { return 1; }
int introduced_in_32_bit(void) __INTRODUCED_IN_32(30);
int introduced_in_32_bit(void) { return 2; }
int deprecated_early(void) __DEPRECATED_IN(21, "use something else");
int deprecated_early(void) { return 3; }
int removed_early(void) __REMOVED_IN(24, "gone");
int removed_early(void) { return 4; }
__END_DECLS

TEST(LlvmLibcAndroidVersioningTest, EveryLevelIsAvailable) {
  EXPECT_EQ(__BIONIC_AVAILABILITY_GUARD(21), 1);
  EXPECT_EQ(__BIONIC_AVAILABILITY_GUARD(35), 1);
  EXPECT_EQ(__BIONIC_AVAILABILITY_GUARD(10000), 1);
}

TEST(LlvmLibcAndroidVersioningTest, ApiLevelMacrosComeWithCdefs) {
  EXPECT_EQ(__ANDROID_API_FUTURE__, 10000);
  EXPECT_GE(__ANDROID_API__, __ANDROID_API_V__);
}

TEST(LlvmLibcAndroidVersioningTest, KernelTypesComeWithCdefs) {
  __u32 word = 0xffffffffu;
  __s64 wide = -1;
  __kernel_pid_t pid = 1;
  EXPECT_EQ(sizeof(word), size_t(4));
  EXPECT_EQ(sizeof(wide), size_t(8));
  EXPECT_EQ(pid, 1);
}

static int ignores_its_argument(int value __unused) { return 5; }

TEST(LlvmLibcAndroidVersioningTest, UnusedAttributeIsDefined) {
  EXPECT_EQ(ignores_its_argument(1), 5);
}

TEST(LlvmLibcAndroidVersioningTest, BionicIncludesComeWithCdefs) {
  off64_t offset = 0;
  EXPECT_EQ(sizeof(offset), size_t(8));
  EXPECT_EQ(F_SETFD, 2);
  EXPECT_EQ(F_GETFD, 1);
}

TEST(LlvmLibcAndroidVersioningTest, MarkedDeclarationsAreUsable) {
  EXPECT_EQ(introduced_late(), 1);
  EXPECT_EQ(introduced_in_32_bit(), 2);
  EXPECT_EQ(deprecated_early(), 3);
  EXPECT_EQ(removed_early(), 4);
}
