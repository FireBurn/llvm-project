//===-- Unittests for inotify ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_inotify_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_inotify_event.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/sys/inotify/inotify_add_watch.h"
#include "src/sys/inotify/inotify_init.h"
#include "src/sys/inotify/inotify_init1.h"
#include "src/sys/inotify/inotify_rm_watch.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "src/unistd/rmdir.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcInotifyTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

TEST_F(LlvmLibcInotifyTest, ReportsAFileBeingMade) {
  constexpr const char *DIR = "inotify.testdir";
  LIBC_NAMESPACE::rmdir(DIR);
  libc_errno = 0;
  ASSERT_THAT(LIBC_NAMESPACE::mkdir(DIR, S_IRWXU), Succeeds(0));

  int fd = LIBC_NAMESPACE::inotify_init1(IN_NONBLOCK);
  ASSERT_GT(fd, 0);
  ASSERT_ERRNO_SUCCESS();

  int wd = LIBC_NAMESPACE::inotify_add_watch(fd, DIR, IN_CREATE);
  ASSERT_GT(wd, 0);

  int made =
      LIBC_NAMESPACE::open("inotify.testdir/new", O_WRONLY | O_CREAT, S_IRWXU);
  ASSERT_GT(made, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(made), Succeeds(0));

  // The event holds the name after the structure rather than a pointer to
  // it, and len says how much room it took.
  alignas(struct inotify_event) char buffer[1024];
  ssize_t got = LIBC_NAMESPACE::read(fd, buffer, sizeof(buffer));
  ASSERT_GT(got, ssize_t(0));
  auto *event = reinterpret_cast<struct inotify_event *>(buffer);
  EXPECT_EQ(event->wd, wd);
  EXPECT_TRUE((event->mask & IN_CREATE) != 0);
  ASSERT_GT(event->len, 0u);
  EXPECT_TRUE(string_view(event->name) == "new");

  ASSERT_THAT(LIBC_NAMESPACE::inotify_rm_watch(fd, wd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::remove("inotify.testdir/new"), 0);
  ASSERT_THAT(LIBC_NAMESPACE::rmdir(DIR), Succeeds(0));
}

TEST_F(LlvmLibcInotifyTest, NothingToReadYet) {
  int fd = LIBC_NAMESPACE::inotify_init1(IN_NONBLOCK);
  ASSERT_GT(fd, 0);

  char buffer[1024];
  EXPECT_THAT(LIBC_NAMESPACE::read(fd, buffer, sizeof(buffer)),
              Fails(EAGAIN, ssize_t(-1)));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcInotifyTest, BadArguments) {
  int fd = LIBC_NAMESPACE::inotify_init1(IN_NONBLOCK);
  ASSERT_GT(fd, 0);

  EXPECT_THAT(
      LIBC_NAMESPACE::inotify_add_watch(fd, "inotify.no.such", IN_CREATE),
      Fails(ENOENT, -1));
  // A watch descriptor which was never given out cannot be removed.
  EXPECT_THAT(LIBC_NAMESPACE::inotify_rm_watch(fd, 9999), Fails(EINVAL, -1));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  EXPECT_THAT(LIBC_NAMESPACE::inotify_add_watch(-1, ".", IN_CREATE),
              Fails(EBADF, -1));
}

// inotify_init is inotify_init1 with no flags, which is how it was before
// the flags were added.
TEST_F(LlvmLibcInotifyTest, InitWithoutFlags) {
  int fd = LIBC_NAMESPACE::inotify_init();
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}
