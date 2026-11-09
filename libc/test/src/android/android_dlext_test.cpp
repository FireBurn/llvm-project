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
#include "src/android/android_dlwarning.h"
#include "src/android/android_get_LD_LIBRARY_PATH.h"
#include "src/android/android_get_exported_namespace.h"
#include "src/android/android_init_anonymous_namespace.h"
#include "src/android/android_link_namespaces.h"
#include "src/android/android_set_16kb_appcompat_mode.h"
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

TEST(LlvmLibcAndroidDlextTest, TheDefaultNamespaceIsExported) {
  struct android_namespace_t *ns =
      LIBC_NAMESPACE::android_get_exported_namespace("default");
  ASSERT_TRUE(ns != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_get_exported_namespace("default") == ns);
  EXPECT_TRUE(LIBC_NAMESPACE::android_get_exported_namespace("sphal") ==
              nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_get_exported_namespace(nullptr) ==
              nullptr);
}

TEST(LlvmLibcAndroidDlextTest, CreatedNamespacesAreNotExported) {
  struct android_namespace_t *ns = LIBC_NAMESPACE::android_create_namespace(
      "test_private", "/lib", nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED, nullptr,
      nullptr);
  ASSERT_TRUE(ns != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_get_exported_namespace("test_private") ==
              nullptr);
}

TEST(LlvmLibcAndroidDlextTest, ANamespaceNeedsAUniqueName) {
  EXPECT_TRUE(LIBC_NAMESPACE::android_create_namespace(
                  nullptr, nullptr, nullptr, 0, nullptr, nullptr) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_create_namespace(
                  "default", nullptr, nullptr, 0, nullptr, nullptr) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  struct android_namespace_t *first = LIBC_NAMESPACE::android_create_namespace(
      "test_unique", nullptr, nullptr, 0, nullptr, nullptr);
  EXPECT_TRUE(first != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_create_namespace("test_unique", nullptr,
                                                       nullptr, 0, nullptr,
                                                       nullptr) == nullptr);
}

TEST(LlvmLibcAndroidDlextTest, AParentThatIsNotANamespaceIsRefused) {
  int not_a_namespace = 0;
  EXPECT_TRUE(LIBC_NAMESPACE::android_create_namespace(
                  "test_bad_parent", nullptr, nullptr, 0, nullptr,
                  reinterpret_cast<struct android_namespace_t *>(
                      &not_a_namespace)) == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, LinkingNeedsTwoNamespacesAndNames) {
  struct android_namespace_t *a = LIBC_NAMESPACE::android_create_namespace(
      "test_link_a", nullptr, nullptr, 0, nullptr, nullptr);
  struct android_namespace_t *b = LIBC_NAMESPACE::android_create_namespace(
      "test_link_b", nullptr, nullptr, 0, nullptr, nullptr);
  ASSERT_TRUE(a != nullptr);
  ASSERT_TRUE(b != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::android_link_namespaces(a, b, "libx.so:liby.so"));
  EXPECT_TRUE(LIBC_NAMESPACE::android_link_namespaces(a, nullptr, "libc.so"));
  EXPECT_FALSE(LIBC_NAMESPACE::android_link_namespaces(a, a, "libx.so"));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  EXPECT_FALSE(LIBC_NAMESPACE::android_link_namespaces(a, b, nullptr));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
  int not_a_namespace = 0;
  EXPECT_FALSE(LIBC_NAMESPACE::android_link_namespaces(
      a, reinterpret_cast<struct android_namespace_t *>(&not_a_namespace),
      "libx.so"));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, TheAnonymousNamespaceIsMadeOnce) {
  EXPECT_TRUE(LIBC_NAMESPACE::android_init_anonymous_namespace(
      "libc.so.1:libm.so.1", "/system/lib64"));
  EXPECT_FALSE(LIBC_NAMESPACE::android_init_anonymous_namespace(
      "libc.so.1", "/system/lib64"));
  EXPECT_TRUE(LIBC_NAMESPACE::dlerror() != nullptr);
}

TEST(LlvmLibcAndroidDlextTest, UpdatingTheLibraryPathSetsTheVariable) {
  LIBC_NAMESPACE::android_update_LD_LIBRARY_PATH("/system/lib64:/vendor/lib64");
  const char *value = LIBC_NAMESPACE::getenv("LD_LIBRARY_PATH");
  ASSERT_TRUE(value != nullptr);
  EXPECT_STREQ(value, "/system/lib64:/vendor/lib64");
}

TEST(LlvmLibcAndroidDlextTest, TheLibraryPathCanBeRead) {
  LIBC_NAMESPACE::android_update_LD_LIBRARY_PATH("/a:/b");
  char buffer[16];
  LIBC_NAMESPACE::android_get_LD_LIBRARY_PATH(buffer, sizeof(buffer));
  EXPECT_STREQ(buffer, "/a:/b");
  char small[3];
  LIBC_NAMESPACE::android_get_LD_LIBRARY_PATH(small, sizeof(small));
  EXPECT_STREQ(small, "/a");
}

static bool warned;

TEST(LlvmLibcAndroidDlextTest, ThereAreNoWarningsToReport) {
  warned = false;
  LIBC_NAMESPACE::android_dlwarning(
      nullptr, [](void *, const char *) { warned = true; });
  EXPECT_FALSE(warned);
}

TEST(LlvmLibcAndroidDlextTest, TheAppCompatModeIsAccepted) {
  LIBC_NAMESPACE::android_set_16kb_appcompat_mode(true);
  LIBC_NAMESPACE::android_set_16kb_appcompat_mode(false);
}
