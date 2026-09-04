//===-- Unittests for getpwnam_r and getpwuid_r ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/pwd/getpwent.h"
#include "src/pwd/getpwnam_r.h"
#include "src/pwd/getpwuid_r.h"
#include "src/pwd/pwd_utils.h"
#include "src/pwd/setpwent.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetPwNamRTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *PASSWD_FILE = "getpwnam_r.test.passwd";

void write_passwd_file() {
  int fd =
      LIBC_NAMESPACE::open(PASSWD_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents("root:x:0:0:root:/root:/bin/sh\n"
                       "andy:x:1000:1000:Andy:/home/andy:/bin/bash\n"
                       "nobody:x:65534:65534::/:/sbin/nologin\n");
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(PASSWD_FILE);
}

} // anonymous namespace

TEST_F(LlvmLibcGetPwNamRTest, FindsByNameAndByUid) {
  write_passwd_file();

  char buffer[1024];
  struct passwd entry;
  struct passwd *result = nullptr;

  ASSERT_EQ(LIBC_NAMESPACE::getpwnam_r("andy", &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result == &entry);
  EXPECT_TRUE(string_view(entry.pw_name) == "andy");
  EXPECT_EQ(entry.pw_uid, uid_t(1000));
  EXPECT_TRUE(string_view(entry.pw_dir) == "/home/andy");

  ASSERT_EQ(LIBC_NAMESPACE::getpwuid_r(65534, &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result == &entry);
  EXPECT_TRUE(string_view(entry.pw_name) == "nobody");

  ASSERT_EQ(LIBC_NAMESPACE::remove(PASSWD_FILE), 0);
}

TEST_F(LlvmLibcGetPwNamRTest, NotFoundIsNotAnError) {
  write_passwd_file();

  char buffer[1024];
  struct passwd entry;
  struct passwd *result = &entry;

  // POSIX says a name which is not there is zero with a null result, not an
  // error.
  EXPECT_EQ(LIBC_NAMESPACE::getpwnam_r("no-such-user", &entry, buffer,
                                       sizeof(buffer), &result),
            0);
  EXPECT_TRUE(result == nullptr);

  result = &entry;
  EXPECT_EQ(
      LIBC_NAMESPACE::getpwuid_r(4242, &entry, buffer, sizeof(buffer), &result),
      0);
  EXPECT_TRUE(result == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(PASSWD_FILE), 0);
}

TEST_F(LlvmLibcGetPwNamRTest, ABufferTooSmallIsReported) {
  write_passwd_file();

  char buffer[8];
  struct passwd entry;
  struct passwd *result = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getpwnam_r("andy", &entry, buffer, sizeof(buffer),
                                       &result),
            ERANGE);
  EXPECT_TRUE(result == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(PASSWD_FILE), 0);
}

TEST_F(LlvmLibcGetPwNamRTest, DoesNotDisturbAnIteration) {
  write_passwd_file();

  LIBC_NAMESPACE::setpwent();
  struct passwd *first = LIBC_NAMESPACE::getpwent();
  ASSERT_TRUE(first != nullptr);
  EXPECT_TRUE(string_view(first->pw_name) == "root");

  // The reentrant lookup opens the file for itself, so the iteration stays
  // where it was, which is what separates it from getpwnam.
  char buffer[1024];
  struct passwd entry;
  struct passwd *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getpwnam_r("nobody", &entry, buffer, sizeof(buffer),
                                       &result),
            0);
  ASSERT_TRUE(result != nullptr);

  struct passwd *second = LIBC_NAMESPACE::getpwent();
  ASSERT_TRUE(second != nullptr);
  EXPECT_TRUE(string_view(second->pw_name) == "andy");

  ASSERT_EQ(LIBC_NAMESPACE::remove(PASSWD_FILE), 0);
}

TEST_F(LlvmLibcGetPwNamRTest, BadArguments) {
  char buffer[1024];
  struct passwd entry;
  struct passwd *result = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getpwnam_r(nullptr, &entry, buffer, sizeof(buffer),
                                       &result),
            EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::getpwnam_r("root", nullptr, buffer, sizeof(buffer),
                                       &result),
            EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::getpwnam_r("root", &entry, nullptr, 0, &result),
            EINVAL);
}
