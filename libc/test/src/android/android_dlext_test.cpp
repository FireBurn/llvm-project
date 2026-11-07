//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android/dlext.h.
///
//===----------------------------------------------------------------------===//

#include "hdr/types/android_dlextinfo.h"
#include "include/llvm-libc-macros/android-dlext-macros.h"
#include "src/android/android_create_namespace.h"
#include "src/android/android_dlopen_ext.h"
#include "src/android/android_get_exported_namespace.h"
#include "src/android/android_init_anonymous_namespace.h"
#include "src/android/android_link_namespaces.h"
#include "src/android/android_update_LD_LIBRARY_PATH.h"
#include "src/dlfcn/dlerror.h"
#include "src/stdlib/getenv.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAndroidDlextTest, FlagValues) {
  EXPECT_EQ(ANDROID_DLEXT_RESERVED_ADDRESS, 0x1);
  EXPECT_EQ(ANDROID_DLEXT_USE_LIBRARY_FD, 0x10);
  EXPECT_EQ(ANDROID_DLEXT_USE_NAMESPACE, 0x200);
  EXPECT_EQ(ANDROID_DLEXT_VALID_FLAG_BITS, 0x67f);
}

TEST(LlvmLibcAndroidDlextTest, InfoLayout) {
  android_dlextinfo info = {};
  EXPECT_EQ(sizeof(info.flags), size_t(8));
  EXPECT_EQ(sizeof(info.library_fd_offset), size_t(8));
  EXPECT_TRUE(info.library_namespace == nullptr);
}

TEST(LlvmLibcAndroidDlextTest, ExtensionsAreRefused) {
  android_dlextinfo info = {};
  info.flags = ANDROID_DLEXT_USE_LIBRARY_FD;
  info.library_fd = 0;
  EXPECT_TRUE(LIBC_NAMESPACE::android_dlopen_ext("libc.so.1", 0, &info) ==
              nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, NoExtensionsFallsBackToDlopen) {
  android_dlextinfo info = {};
  EXPECT_TRUE(LIBC_NAMESPACE::android_dlopen_ext("/nonexistent/libx.so", 0,
                                                 &info) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_dlopen_ext("/nonexistent/libx.so", 0,
                                                 nullptr) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, NamespacesAreNotSupported) {
  EXPECT_TRUE(LIBC_NAMESPACE::android_get_exported_namespace("sphal") ==
              nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_create_namespace(
                  "ns", nullptr, nullptr, 0, nullptr, nullptr) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  EXPECT_FALSE(LIBC_NAMESPACE::android_link_namespaces(nullptr, nullptr, ""));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  EXPECT_FALSE(LIBC_NAMESPACE::android_init_anonymous_namespace("", ""));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, UpdatingTheLibraryPathSetsTheVariable) {
  LIBC_NAMESPACE::android_update_LD_LIBRARY_PATH("/system/lib64:/vendor/lib64");
  const char *value = LIBC_NAMESPACE::getenv("LD_LIBRARY_PATH");
  ASSERT_TRUE(value != nullptr);
  EXPECT_STREQ(value, "/system/lib64:/vendor/lib64");
}
