//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android_get_device_api_level.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_get_device_api_level.h"
#include "src/__support/libc_errno.h"
#include "src/string/strlen.h"
#include "src/sys/system_properties/__system_property_add.h"
#include "src/sys/system_properties/__system_property_area_init.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/__system_property_update.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAndroidDeviceApiLevelTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// One test, as the property area is process wide and cannot be made twice.
TEST_F(LlvmLibcAndroidDeviceApiLevelTest, ReadsTheSdkProperty) {
  const char *path = libc_make_test_file_path("device_api_level_area");
  // A file left by an earlier run would make the area exist already.
  LIBC_NAMESPACE::unlink(path);
  libc_errno = 0;
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(path), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_area_init(), 0);

  EXPECT_EQ(LIBC_NAMESPACE::android_get_device_api_level(), -1);

  const char *name = "ro.build.version.sdk";
  const char *level = "34";
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_add(
                name, static_cast<unsigned>(LIBC_NAMESPACE::strlen(name)),
                level, static_cast<unsigned>(LIBC_NAMESPACE::strlen(level))),
            0);
  EXPECT_EQ(LIBC_NAMESPACE::android_get_device_api_level(), 34);

  prop_info *property = const_cast<prop_info *>(
      LIBC_NAMESPACE::__system_property_find(name));
  ASSERT_TRUE(property != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_update(property, "x7", 2), 0);
  EXPECT_EQ(LIBC_NAMESPACE::android_get_device_api_level(), -1);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_update(property, "0", 1), 0);
  EXPECT_EQ(LIBC_NAMESPACE::android_get_device_api_level(), -1);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_update(property, "36", 2), 0);
  EXPECT_EQ(LIBC_NAMESPACE::android_get_device_api_level(), 36);

  LIBC_NAMESPACE::unlink(path);
}
