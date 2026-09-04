//===-- Unittests for getgrnam_r and getgrgid_r ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/grp/getgrent.h"
#include "src/grp/getgrgid_r.h"
#include "src/grp/getgrnam_r.h"
#include "src/grp/grp_utils.h"
#include "src/grp/setgrent.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetGrNamRTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *GROUP_FILE = "getgrnam_r.test.group";

void write_group_file() {
  int fd =
      LIBC_NAMESPACE::open(GROUP_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents("root:x:0:\n"
                       "wheel:x:10:root,andy,jo\n"
                       "audio:x:18:andy\n");
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  LIBC_NAMESPACE::group::TESTONLY_set_group_path(GROUP_FILE);
}

} // anonymous namespace

TEST_F(LlvmLibcGetGrNamRTest, FindsByNameAndByGid) {
  write_group_file();

  char buffer[1024];
  struct group entry;
  struct group *result = nullptr;

  ASSERT_EQ(LIBC_NAMESPACE::getgrnam_r("wheel", &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result == &entry);
  EXPECT_TRUE(string_view(entry.gr_name) == "wheel");
  EXPECT_EQ(entry.gr_gid, gid_t(10));

  ASSERT_EQ(
      LIBC_NAMESPACE::getgrgid_r(18, &entry, buffer, sizeof(buffer), &result),
      0);
  ASSERT_TRUE(result == &entry);
  EXPECT_TRUE(string_view(entry.gr_name) == "audio");

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamRTest, TheMemberListIsInTheCallersBuffer) {
  write_group_file();

  char buffer[1024];
  struct group entry;
  struct group *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getgrnam_r("wheel", &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result != nullptr);

  // Both the names and the array of pointers to them come out of the buffer
  // the caller gave, which is what makes this reentrant.
  ASSERT_TRUE(entry.gr_mem != nullptr);
  EXPECT_TRUE(string_view(entry.gr_mem[0]) == "root");
  EXPECT_TRUE(string_view(entry.gr_mem[1]) == "andy");
  EXPECT_TRUE(string_view(entry.gr_mem[2]) == "jo");
  EXPECT_TRUE(entry.gr_mem[3] == nullptr);

  auto within = [&](const void *p) {
    return p >= static_cast<const void *>(buffer) &&
           p < static_cast<const void *>(buffer + sizeof(buffer));
  };
  EXPECT_TRUE(within(entry.gr_mem));
  EXPECT_TRUE(within(entry.gr_mem[0]));

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamRTest, NotFoundIsNotAnError) {
  write_group_file();

  char buffer[1024];
  struct group entry;
  struct group *result = &entry;
  EXPECT_EQ(LIBC_NAMESPACE::getgrnam_r("no-such-group", &entry, buffer,
                                       sizeof(buffer), &result),
            0);
  EXPECT_TRUE(result == nullptr);

  result = &entry;
  EXPECT_EQ(
      LIBC_NAMESPACE::getgrgid_r(4242, &entry, buffer, sizeof(buffer), &result),
      0);
  EXPECT_TRUE(result == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamRTest, ABufferTooSmallIsReported) {
  write_group_file();

  char buffer[8];
  struct group entry;
  struct group *result = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getgrnam_r("wheel", &entry, buffer, sizeof(buffer),
                                       &result),
            ERANGE);
  EXPECT_TRUE(result == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamRTest, DoesNotDisturbAnIteration) {
  write_group_file();

  LIBC_NAMESPACE::setgrent();
  struct group *first = LIBC_NAMESPACE::getgrent();
  ASSERT_TRUE(first != nullptr);
  EXPECT_TRUE(string_view(first->gr_name) == "root");

  // The reentrant lookup opens the file for itself, so the iteration stays
  // where it was, which is what separates it from getgrnam.
  char buffer[1024];
  struct group entry;
  struct group *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getgrnam_r("audio", &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result != nullptr);

  struct group *second = LIBC_NAMESPACE::getgrent();
  ASSERT_TRUE(second != nullptr);
  EXPECT_TRUE(string_view(second->gr_name) == "wheel");

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamRTest, BadArguments) {
  char buffer[1024];
  struct group entry;
  struct group *result = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getgrnam_r(nullptr, &entry, buffer, sizeof(buffer),
                                       &result),
            EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::getgrnam_r("root", nullptr, buffer, sizeof(buffer),
                                       &result),
            EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::getgrnam_r("root", &entry, nullptr, 0, &result),
            EINVAL);
}
