//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the android_fdsan functions.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/types/enum_android_fdsan_error_level.h"
#include "hdr/types/enum_android_fdsan_owner_type.h"
#include "src/android/android_fdsan_close_with_tag.h"
#include "src/android/android_fdsan_create_owner_tag.h"
#include "src/android/android_fdsan_exchange_owner_tag.h"
#include "src/android/android_fdsan_get_error_level.h"
#include "src/android/android_fdsan_get_owner_tag.h"
#include "src/android/android_fdsan_get_tag_type.h"
#include "src/android/android_fdsan_get_tag_value.h"
#include "src/android/android_fdsan_set_error_level.h"
#include "src/android/android_fdsan_set_error_level_from_property.h"
#include "src/fcntl/open.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAndroidFdsanTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcAndroidFdsanTest, TagHoldsTheTypeAboveTheValue) {
  uint64_t tag = LIBC_NAMESPACE::android_fdsan_create_owner_tag(
      ANDROID_FDSAN_OWNER_TYPE_FILE, 0x1234);
  EXPECT_EQ(tag, (uint64_t(1) << 56) | 0x1234);
  EXPECT_EQ(LIBC_NAMESPACE::android_fdsan_get_tag_value(tag), uint64_t(0x1234));
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(
                LIBC_NAMESPACE::android_fdsan_get_tag_type(tag), "FILE*"),
            0);
}

TEST_F(LlvmLibcAndroidFdsanTest, ZeroValueGivesNoTag) {
  EXPECT_EQ(LIBC_NAMESPACE::android_fdsan_create_owner_tag(
                ANDROID_FDSAN_OWNER_TYPE_DIR, 0),
            uint64_t(0));
}

TEST_F(LlvmLibcAndroidFdsanTest, ValueBeyondFiftySixBitsIsCut) {
  uint64_t tag = LIBC_NAMESPACE::android_fdsan_create_owner_tag(
      ANDROID_FDSAN_OWNER_TYPE_UNIQUE_FD, ~uint64_t(0));
  EXPECT_EQ(LIBC_NAMESPACE::android_fdsan_get_tag_value(tag),
            (uint64_t(1) << 56) - 1);
  EXPECT_EQ(tag >> 56, uint64_t(ANDROID_FDSAN_OWNER_TYPE_UNIQUE_FD));
}

TEST_F(LlvmLibcAndroidFdsanTest, TagTypeNames) {
  const char *expected[] = {"generic",
                            "FILE*",
                            "DIR*",
                            "unique_fd",
                            "sqlite",
                            "FileInputStream",
                            "FileOutputStream",
                            "RandomAccessFile",
                            "ParcelFileDescriptor",
                            "ArtFd",
                            "DatagramSocketImpl",
                            "SocketImpl",
                            "ZipArchive",
                            "native_handle_t",
                            "Parcel"};
  for (uint64_t type = 0; type < 15; ++type) {
    const char *name =
        LIBC_NAMESPACE::android_fdsan_get_tag_type(type << 56 | 1);
    ASSERT_EQ(LIBC_NAMESPACE::strcmp(name, expected[type]), 0);
  }
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(
                LIBC_NAMESPACE::android_fdsan_get_tag_type(uint64_t(200) << 56),
                "unknown"),
            0);
}

TEST_F(LlvmLibcAndroidFdsanTest, CloseWithAnyTagClosesTheDescriptor) {
  int fd = LIBC_NAMESPACE::open("/dev/null", O_RDONLY);
  ASSERT_GE(fd, 0);
  uint64_t tag = LIBC_NAMESPACE::android_fdsan_create_owner_tag(
      ANDROID_FDSAN_OWNER_TYPE_FILE, 7);
  EXPECT_EQ(LIBC_NAMESPACE::android_fdsan_get_owner_tag(fd), uint64_t(0));
  LIBC_NAMESPACE::android_fdsan_exchange_owner_tag(fd, 0, tag);
  ASSERT_THAT(LIBC_NAMESPACE::android_fdsan_close_with_tag(fd, tag),
              Succeeds());
  ASSERT_THAT(LIBC_NAMESPACE::android_fdsan_close_with_tag(fd, tag),
              Fails(EBADF));
}

TEST_F(LlvmLibcAndroidFdsanTest, ErrorLevelIsKeptAndTheOldOneReturned) {
  using namespace LIBC_NAMESPACE;
  EXPECT_EQ(android_fdsan_get_error_level(),
            ANDROID_FDSAN_ERROR_LEVEL_DISABLED);
  EXPECT_EQ(android_fdsan_set_error_level(ANDROID_FDSAN_ERROR_LEVEL_FATAL),
            ANDROID_FDSAN_ERROR_LEVEL_DISABLED);
  EXPECT_EQ(android_fdsan_get_error_level(), ANDROID_FDSAN_ERROR_LEVEL_FATAL);
  // A level that was chosen stands over the default.
  EXPECT_EQ(android_fdsan_set_error_level_from_property(
                ANDROID_FDSAN_ERROR_LEVEL_WARN_ONCE),
            ANDROID_FDSAN_ERROR_LEVEL_FATAL);
  EXPECT_EQ(android_fdsan_set_error_level(ANDROID_FDSAN_ERROR_LEVEL_DISABLED),
            ANDROID_FDSAN_ERROR_LEVEL_FATAL);
}
