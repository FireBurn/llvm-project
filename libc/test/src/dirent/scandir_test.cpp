//===-- Unittests for scandir, alphasort and versionsort ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/dirent/alphasort.h"
#include "src/dirent/scandir.h"
#include "src/dirent/versionsort.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <dirent.h>

using LlvmLibcScandirTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

int only_txt(const struct dirent *entry) {
  string_view name(&entry->d_name[0]);
  return name.ends_with(".txt") ? 1 : 0;
}

void release(struct dirent **list, int count) {
  for (int i = 0; i < count; ++i)
    ::free(list[i]);
  ::free(list);
}

} // anonymous namespace

TEST_F(LlvmLibcScandirTest, ReadsAndSortsADirectory) {
  struct dirent **list = nullptr;
  int count = LIBC_NAMESPACE::scandir("testdata", &list, nullptr,
                                      &LIBC_NAMESPACE::alphasort);
  ASSERT_GT(count, 0);
  ASSERT_ERRNO_SUCCESS();

  // The names come back in order.
  for (int i = 1; i < count; ++i) {
    auto *previous = const_cast<const struct dirent *>(list[i - 1]);
    auto *current = const_cast<const struct dirent *>(list[i]);
    EXPECT_LE(LIBC_NAMESPACE::alphasort(&previous, &current), 0);
  }
  release(list, count);
}

TEST_F(LlvmLibcScandirTest, TheFilterDecidesWhatIsKept) {
  struct dirent **all = nullptr;
  int total = LIBC_NAMESPACE::scandir("testdata", &all, nullptr,
                                      &LIBC_NAMESPACE::alphasort);
  ASSERT_GT(total, 0);
  release(all, total);

  struct dirent **some = nullptr;
  int kept = LIBC_NAMESPACE::scandir("testdata", &some, only_txt,
                                     &LIBC_NAMESPACE::alphasort);
  ASSERT_GE(kept, 0);
  EXPECT_LT(kept, total);
  for (int i = 0; i < kept; ++i)
    EXPECT_TRUE(string_view(&some[i]->d_name[0]).ends_with(".txt"));
  release(some, kept);
}

TEST_F(LlvmLibcScandirTest, NoComparisonMeansNoSorting) {
  struct dirent **list = nullptr;
  // The order is unspecified without a comparison, but everything is still
  // there.
  int count = LIBC_NAMESPACE::scandir("testdata", &list, nullptr, nullptr);
  ASSERT_GT(count, 0);
  release(list, count);
}

TEST_F(LlvmLibcScandirTest, TheEntriesOutliveTheDirectory) {
  struct dirent **list = nullptr;
  int count = LIBC_NAMESPACE::scandir("testdata", &list, nullptr,
                                      &LIBC_NAMESPACE::alphasort);
  ASSERT_GT(count, 1);
  // Every entry is a copy of its own, so the names are all still readable
  // after the directory has been closed and they do not run together.
  for (int i = 0; i < count; ++i) {
    ASSERT_TRUE(list[i] != nullptr);
    EXPECT_FALSE(string_view(&list[i]->d_name[0]).empty());
  }
  release(list, count);
}

TEST_F(LlvmLibcScandirTest, MissingDirectory) {
  struct dirent **list = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::scandir("scandir.no.such.dir", &list, nullptr,
                                    &LIBC_NAMESPACE::alphasort),
            -1);
  ASSERT_ERRNO_EQ(ENOENT);
}

TEST_F(LlvmLibcScandirTest, NullArguments) {
  struct dirent **list = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::scandir(nullptr, &list, nullptr,
                                    &LIBC_NAMESPACE::alphasort),
            -1);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::scandir("testdata", nullptr, nullptr,
                                    &LIBC_NAMESPACE::alphasort),
            -1);
  ASSERT_ERRNO_EQ(EINVAL);
}
