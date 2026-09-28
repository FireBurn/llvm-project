//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unit tests for removexattr, lremovexattr and fremovexattr.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_stat_macros.h"
#include "hdr/sys_xattr_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/CPP/scope.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/creat.h"
#include "src/sys/xattr/fgetxattr.h"
#include "src/sys/xattr/fremovexattr.h"
#include "src/sys/xattr/fsetxattr.h"
#include "src/sys/xattr/getxattr.h"
#include "src/sys/xattr/lremovexattr.h"
#include "src/sys/xattr/removexattr.h"
#include "src/sys/xattr/setxattr.h"
#include "src/unistd/close.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

namespace {

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcRemovexattrTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::cpp::scope_exit;

constexpr const char *NAME = "user.removexattr_test";
constexpr char VALUE[] = "value";

int recreate_test_file(const char *path) {
  LIBC_NAMESPACE::unlink(path);
  LIBC_NAMESPACE::libc_errno = 0;
  return LIBC_NAMESPACE::creat(path, S_IRWXU);
}

TEST_F(LlvmLibcRemovexattrTest, ByPath) {
  const LIBC_NAMESPACE::CString TEST_FILE =
      libc_make_test_file_path("testdata/removexattr_path.txt");
  int fd = recreate_test_file(TEST_FILE);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  scope_exit cleanup(
      [&] { ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0)); });

  int ret = LIBC_NAMESPACE::setxattr(TEST_FILE, NAME, VALUE, sizeof(VALUE), 0);
  if (ret != 0) {
    // The filesystem the tests run on may not take user attributes.
    ASSERT_ERRNO_EQ(ENOTSUP);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_THAT(LIBC_NAMESPACE::removexattr(TEST_FILE, NAME), Succeeds(0));
  char buffer[16];
  EXPECT_THAT(
      LIBC_NAMESPACE::getxattr(TEST_FILE, NAME, buffer, sizeof(buffer)),
      Fails<ssize_t>(ENODATA));
  EXPECT_THAT(LIBC_NAMESPACE::removexattr(TEST_FILE, NAME), Fails(ENODATA));
}

TEST_F(LlvmLibcRemovexattrTest, ByDescriptor) {
  const LIBC_NAMESPACE::CString TEST_FILE =
      libc_make_test_file_path("testdata/removexattr_fd.txt");
  int fd = recreate_test_file(TEST_FILE);
  ASSERT_ERRNO_SUCCESS();
  scope_exit cleanup([&] {
    ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
    ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0));
  });

  int ret = LIBC_NAMESPACE::fsetxattr(fd, NAME, VALUE, sizeof(VALUE), 0);
  if (ret != 0) {
    ASSERT_ERRNO_EQ(ENOTSUP);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_THAT(LIBC_NAMESPACE::fremovexattr(fd, NAME), Succeeds(0));
  char buffer[16];
  EXPECT_THAT(LIBC_NAMESPACE::fgetxattr(fd, NAME, buffer, sizeof(buffer)),
              Fails<ssize_t>(ENODATA));
}

TEST_F(LlvmLibcRemovexattrTest, LinkItselfNotItsTarget) {
  const LIBC_NAMESPACE::CString TEST_FILE =
      libc_make_test_file_path("testdata/removexattr_target.txt");
  const LIBC_NAMESPACE::CString TEST_LINK =
      libc_make_test_file_path("testdata/removexattr_link");
  int fd = recreate_test_file(TEST_FILE);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  LIBC_NAMESPACE::unlink(TEST_LINK);
  LIBC_NAMESPACE::libc_errno = 0;
  ASSERT_THAT(LIBC_NAMESPACE::symlink("removexattr_target.txt", TEST_LINK),
              Succeeds(0));
  scope_exit cleanup([&] {
    ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_LINK), Succeeds(0));
    ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0));
  });

  int ret = LIBC_NAMESPACE::setxattr(TEST_FILE, NAME, VALUE, sizeof(VALUE), 0);
  if (ret != 0) {
    ASSERT_ERRNO_EQ(ENOTSUP);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  // The attribute is on the target, and lremovexattr acts on the link
  // itself, so the target keeps it. Linux refuses user attributes on a
  // symbolic link outright rather than finding none there.
  ASSERT_EQ(LIBC_NAMESPACE::lremovexattr(TEST_LINK, NAME), -1);
  ASSERT_TRUE(LIBC_NAMESPACE::libc_errno == EPERM ||
              LIBC_NAMESPACE::libc_errno == ENODATA);
  LIBC_NAMESPACE::libc_errno = 0;
  char buffer[16];
  EXPECT_THAT(
      LIBC_NAMESPACE::getxattr(TEST_FILE, NAME, buffer, sizeof(buffer)),
      Succeeds<ssize_t>(sizeof(VALUE)));
  ASSERT_THAT(LIBC_NAMESPACE::removexattr(TEST_LINK, NAME), Succeeds(0));
}

} // namespace
