//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android_get_application_target_sdk_version.
///
//===----------------------------------------------------------------------===//

#include "hdr/android_api_level_macros.h"
#include "src/android/android_get_application_target_sdk_version.h"
#include "src/android/android_set_application_target_sdk_version.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidApiLevelTest, NativeProgramIsTheNewestLevel) {
  EXPECT_EQ(LIBC_NAMESPACE::android_get_application_target_sdk_version(),
            __ANDROID_API_FUTURE__);
}

TEST(LlvmLibcAndroidApiLevelTest, AnApplicationCanDeclareItsLevel) {
  LIBC_NAMESPACE::android_set_application_target_sdk_version(__ANDROID_API_R__);
  EXPECT_EQ(LIBC_NAMESPACE::android_get_application_target_sdk_version(),
            __ANDROID_API_R__);
  LIBC_NAMESPACE::android_set_application_target_sdk_version(
      __ANDROID_API_FUTURE__);
}

TEST(LlvmLibcAndroidApiLevelTest, LevelsMatchTheReleases) {
  EXPECT_EQ(__ANDROID_API_G__, 9);
  EXPECT_EQ(__ANDROID_API_L__, 21);
  EXPECT_EQ(__ANDROID_API_R__, 30);
  EXPECT_EQ(__ANDROID_API_V__, 35);
  EXPECT_EQ(__ANDROID_API_FUTURE__, 10000);
}
