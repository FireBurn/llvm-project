//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the crash details.
///
//===----------------------------------------------------------------------===//

#include "src/__support/crash_detail.h"
#include "src/android/android_crash_detail_register.h"
#include "src/android/android_crash_detail_replace_data.h"
#include "src/android/android_crash_detail_replace_name.h"
#include "src/android/android_crash_detail_unregister.h"
#include "test/UnitTest/Test.h"

namespace {
namespace cd = LIBC_NAMESPACE::crash_detail;
} // namespace

TEST(LlvmLibcCrashDetailTest, RegisterKeepsThePointers) {
  const char name[] = "key";
  const char data[] = "value";
  cd::Detail *detail = LIBC_NAMESPACE::android_crash_detail_register(
      name, sizeof(name) - 1, data, sizeof(data) - 1);
  ASSERT_TRUE(detail != nullptr);
  EXPECT_TRUE(detail->name == name);
  EXPECT_EQ(detail->name_size, size_t(3));
  EXPECT_TRUE(detail->data == data);
  EXPECT_EQ(detail->data_size, size_t(5));
  // A crash handler finds it from the newest page.
  const cd::Page *page = cd::newest_page();
  ASSERT_TRUE(page != nullptr);
  EXPECT_TRUE(detail >= page->details &&
              detail < page->details + cd::DETAILS_PER_PAGE);
  LIBC_NAMESPACE::android_crash_detail_unregister(detail);
}

TEST(LlvmLibcCrashDetailTest, ReplaceAndUnregister) {
  const char name[] = "a";
  const char other_name[] = "bb";
  const char data[] = "x";
  const char other_data[] = "yyy";
  cd::Detail *detail =
      LIBC_NAMESPACE::android_crash_detail_register(name, 1, data, 1);
  ASSERT_TRUE(detail != nullptr);
  LIBC_NAMESPACE::android_crash_detail_replace_data(detail, other_data, 3);
  EXPECT_TRUE(detail->data == other_data);
  EXPECT_EQ(detail->data_size, size_t(3));
  LIBC_NAMESPACE::android_crash_detail_replace_name(detail, other_name, 2);
  EXPECT_TRUE(detail->name == other_name);
  EXPECT_EQ(detail->name_size, size_t(2));

  // A handler skips an entry without data, and the entry is used again.
  LIBC_NAMESPACE::android_crash_detail_unregister(detail);
  EXPECT_TRUE(detail->data == nullptr);
  cd::Detail *again =
      LIBC_NAMESPACE::android_crash_detail_register(name, 1, data, 1);
  EXPECT_TRUE(again == detail);
  LIBC_NAMESPACE::android_crash_detail_unregister(again);
}

TEST(LlvmLibcCrashDetailTest, FillsPagesAndKeepsTheOlderOnes) {
  const char name[] = "n";
  const char data[] = "d";
  cd::Detail *details[cd::DETAILS_PER_PAGE + 2];
  for (size_t i = 0; i < cd::DETAILS_PER_PAGE + 2; ++i) {
    details[i] =
        LIBC_NAMESPACE::android_crash_detail_register(name, 1, data, 1);
    ASSERT_TRUE(details[i] != nullptr);
  }
  const cd::Page *newest = cd::newest_page();
  ASSERT_TRUE(newest != nullptr);
  ASSERT_TRUE(newest->prev != nullptr);
  EXPECT_EQ(newest->prev->used, cd::DETAILS_PER_PAGE);
  for (size_t i = 0; i < cd::DETAILS_PER_PAGE + 2; ++i)
    LIBC_NAMESPACE::android_crash_detail_unregister(details[i]);
}
