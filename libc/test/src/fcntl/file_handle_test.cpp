//===-- Unittests for name_to_handle_at and open_by_handle_at -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_file_handle.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/name_to_handle_at.h"
#include "src/fcntl/open.h"
#include "src/fcntl/open_by_handle_at.h"
#include "src/string/memcmp.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFileHandleTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

// The handle is longer than the struct that carries it, so it is made in a
// buffer with room after the fields for the bytes the kernel writes.
union HandleBuffer {
  struct file_handle handle;
  char bytes[sizeof(struct file_handle) + 128];
};

} // anonymous namespace

// A handle names the file wherever it moves to, which is what it is for.
// Opening one again needs CAP_DAC_READ_SEARCH, so a test without it goes as
// far as making the handle and is refused at the open.
TEST_F(LlvmLibcFileHandleTest, MakesAHandleAndOpensItAgain) {
  constexpr char TEXT[] = "file_handle";
  auto path = libc_make_test_file_path("file_handle.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
              Succeeds<ssize_t>(sizeof(TEXT)));
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  HandleBuffer buffer = {};
  buffer.handle.handle_bytes =
      sizeof(buffer.bytes) - sizeof(struct file_handle);
  int mount_id = 0;
  int made = LIBC_NAMESPACE::name_to_handle_at(AT_FDCWD, path, &buffer.handle,
                                               &mount_id, 0);
  if (made != 0) {
    // A filesystem need not be able to name a file this way at all.
    ASSERT_ERRNO_EQ(EOPNOTSUPP);
    LIBC_NAMESPACE::libc_errno = 0;
    ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
    return;
  }
  ASSERT_GT(buffer.handle.handle_bytes, 0U);

  int mount = LIBC_NAMESPACE::open("/", O_RDONLY | O_DIRECTORY);
  ASSERT_GT(mount, 0);
  int opened =
      LIBC_NAMESPACE::open_by_handle_at(mount, &buffer.handle, O_RDONLY);
  if (opened < 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
  } else {
    char read[sizeof(TEXT)] = {};
    ASSERT_THAT(LIBC_NAMESPACE::read(opened, read, sizeof(TEXT)),
                Succeeds<ssize_t>(sizeof(TEXT)));
    ASSERT_EQ(LIBC_NAMESPACE::memcmp(read, TEXT, sizeof(TEXT)), 0);
    ASSERT_THAT(LIBC_NAMESPACE::close(opened), Succeeds(0));
  }

  ASSERT_THAT(LIBC_NAMESPACE::close(mount), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// Where the room given is too small the kernel says how much it needed.
TEST_F(LlvmLibcFileHandleTest, RoomThatIsNotEnough) {
  auto path = libc_make_test_file_path("file_handle_small.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  HandleBuffer buffer = {};
  buffer.handle.handle_bytes = 0;
  int mount_id = 0;
  int result = LIBC_NAMESPACE::name_to_handle_at(AT_FDCWD, path, &buffer.handle,
                                                 &mount_id, 0);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EOVERFLOW || err == EOPNOTSUPP);
  if (err == EOVERFLOW)
    ASSERT_GT(buffer.handle.handle_bytes, 0U);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFileHandleTest, PathThatIsNotThere) {
  HandleBuffer buffer = {};
  buffer.handle.handle_bytes =
      sizeof(buffer.bytes) - sizeof(struct file_handle);
  int mount_id = 0;
  ASSERT_THAT(LIBC_NAMESPACE::name_to_handle_at(AT_FDCWD, "llvm-libc-not-here",
                                                &buffer.handle, &mount_id, 0),
              Fails(ENOENT));
}
