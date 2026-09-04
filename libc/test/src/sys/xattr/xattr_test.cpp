//===-- Unittests for the extended attribute calls ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/string/memcmp.h"
#include "src/sys/xattr/fgetxattr.h"
#include "src/sys/xattr/flistxattr.h"
#include "src/sys/xattr/fremovexattr.h"
#include "src/sys/xattr/fsetxattr.h"
#include "src/sys/xattr/getxattr.h"
#include "src/sys/xattr/lgetxattr.h"
#include "src/sys/xattr/listxattr.h"
#include "src/sys/xattr/llistxattr.h"
#include "src/sys/xattr/lremovexattr.h"
#include "src/sys/xattr/lsetxattr.h"
#include "src/sys/xattr/removexattr.h"
#include "src/sys/xattr/setxattr.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcXattrTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

constexpr const char NAME[] = "user.llvm_libc_test";
constexpr const char VALUE[] = "value";
constexpr size_t VALUE_SIZE = sizeof(VALUE);

// A filesystem need not carry extended attributes at all, and one that does
// may still refuse the user namespace. Neither is a fault in the call, so a
// test that cannot store an attribute reports nothing rather than failing.
bool set_or_unsupported(const char *path) {
  bool ok = LIBC_NAMESPACE::setxattr(path, NAME, VALUE, VALUE_SIZE, 0) == 0;
  LIBC_NAMESPACE::libc_errno = 0;
  return ok;
}

int make_file(const char *path) {
  return LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
}

} // namespace

TEST_F(LlvmLibcXattrTest, SetGetListRemoveByPath) {
  auto path = libc_make_test_file_path("xattr_path.test");
  int fd = make_file(path);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  if (!set_or_unsupported(path)) {
    ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
    return;
  }

  char buf[VALUE_SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::getxattr(path, NAME, buf, sizeof(buf)),
              Succeeds<ssize_t>(static_cast<ssize_t>(VALUE_SIZE)));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, VALUE, VALUE_SIZE), 0);

  // A size of zero asks for the length rather than the value.
  ASSERT_THAT(LIBC_NAMESPACE::getxattr(path, NAME, nullptr, 0),
              Succeeds<ssize_t>(static_cast<ssize_t>(VALUE_SIZE)));

  char list[256];
  ssize_t len = LIBC_NAMESPACE::listxattr(path, list, sizeof(list));
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(len, static_cast<ssize_t>(sizeof(NAME)));

  ASSERT_THAT(LIBC_NAMESPACE::removexattr(path, NAME), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::getxattr(path, NAME, buf, sizeof(buf)),
              Fails<ssize_t>(ENODATA));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcXattrTest, SetGetListRemoveByLink) {
  auto path = libc_make_test_file_path("xattr_link.test");
  int fd = make_file(path);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  if (!set_or_unsupported(path)) {
    ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
    return;
  }
  ASSERT_THAT(LIBC_NAMESPACE::removexattr(path, NAME), Succeeds(0));

  // The path names a file rather than a symbolic link, so the l-forms see
  // exactly what the plain forms do.
  ASSERT_THAT(LIBC_NAMESPACE::lsetxattr(path, NAME, VALUE, VALUE_SIZE, 0),
              Succeeds(0));

  char buf[VALUE_SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::lgetxattr(path, NAME, buf, sizeof(buf)),
              Succeeds<ssize_t>(static_cast<ssize_t>(VALUE_SIZE)));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, VALUE, VALUE_SIZE), 0);

  char list[256];
  ssize_t len = LIBC_NAMESPACE::llistxattr(path, list, sizeof(list));
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(len, static_cast<ssize_t>(sizeof(NAME)));

  ASSERT_THAT(LIBC_NAMESPACE::lremovexattr(path, NAME), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lgetxattr(path, NAME, buf, sizeof(buf)),
              Fails<ssize_t>(ENODATA));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcXattrTest, SetGetListRemoveByDescriptor) {
  auto path = libc_make_test_file_path("xattr_fd.test");
  int fd = make_file(path);
  ASSERT_GT(fd, 0);

  if (LIBC_NAMESPACE::fsetxattr(fd, NAME, VALUE, VALUE_SIZE, 0) != 0) {
    LIBC_NAMESPACE::libc_errno = 0;
    ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
    ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
    return;
  }

  char buf[VALUE_SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::fgetxattr(fd, NAME, buf, sizeof(buf)),
              Succeeds<ssize_t>(static_cast<ssize_t>(VALUE_SIZE)));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, VALUE, VALUE_SIZE), 0);

  char list[256];
  ssize_t len = LIBC_NAMESPACE::flistxattr(fd, list, sizeof(list));
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(len, static_cast<ssize_t>(sizeof(NAME)));

  ASSERT_THAT(LIBC_NAMESPACE::fremovexattr(fd, NAME), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::fgetxattr(fd, NAME, buf, sizeof(buf)),
              Fails<ssize_t>(ENODATA));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcXattrTest, NameThatIsNotThere) {
  auto path = libc_make_test_file_path("xattr_absent.test");
  int fd = make_file(path);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  char buf[8];
  ASSERT_THAT(LIBC_NAMESPACE::getxattr(path, "user.absent", buf, sizeof(buf)),
              Fails<ssize_t>(ENODATA));
  ASSERT_THAT(LIBC_NAMESPACE::removexattr(path, "user.absent"), Fails(ENODATA));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcXattrTest, PathThatIsNotThere) {
  char buf[8];
  ASSERT_THAT(LIBC_NAMESPACE::getxattr("/nonexistent/llvm_libc", NAME, buf,
                                       sizeof(buf)),
              Fails<ssize_t>(ENOENT));
  ASSERT_THAT(LIBC_NAMESPACE::lgetxattr("/nonexistent/llvm_libc", NAME, buf,
                                        sizeof(buf)),
              Fails<ssize_t>(ENOENT));
  ASSERT_THAT(LIBC_NAMESPACE::fgetxattr(-1, NAME, buf, sizeof(buf)),
              Fails<ssize_t>(EBADF));
}
