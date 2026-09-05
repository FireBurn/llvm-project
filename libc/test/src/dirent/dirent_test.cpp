//===-- Unittests for functions from POSIX dirent.h -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/dirent/closedir.h"
#include "src/dirent/dirfd.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/close.h"
#include "src/unistd/rmdir.h"
#include "src/unistd/unlink.h"

#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"

#include <dirent.h>

using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

// The tests list a directory the test makes for itself, so that they do not
// depend on where they are run from.
class LlvmLibcDirentTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
public:
  void SetUp() override {
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::SetUp();
    remove_all();
    ASSERT_EQ(LIBC_NAMESPACE::mkdir(root(), S_IRWXU), 0);
    for (const char *name : FILES) {
      char path[512];
      join(path, name);
      int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT, S_IRWXU);
      ASSERT_GT(fd, 0);
      ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
    }
    for (const char *name : DIRS) {
      char path[512];
      join(path, name);
      ASSERT_EQ(LIBC_NAMESPACE::mkdir(path, S_IRWXU), 0);
    }
    libc_errno = 0;
  }

  void TearDown() override {
    remove_all();
    libc_errno = 0;
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::TearDown();
  }

  const char *root() const { return libc_make_test_file_path("dirent_dir"); }

  void join(char *out, const char *name) const {
    size_t n = 0;
    for (const char *p = root(); *p != '\0'; ++p)
      out[n++] = *p;
    out[n++] = '/';
    for (const char *p = name; *p != '\0'; ++p)
      out[n++] = *p;
    out[n] = '\0';
  }

private:
  static constexpr const char *FILES[] = {"file1.txt", "file2.txt"};
  static constexpr const char *DIRS[] = {"dir1", "dir2"};

  void remove_all() {
    for (const char *name : FILES) {
      char path[512];
      join(path, name);
      LIBC_NAMESPACE::unlink(path);
    }
    for (const char *name : DIRS) {
      char path[512];
      join(path, name);
      LIBC_NAMESPACE::rmdir(path);
    }
    LIBC_NAMESPACE::rmdir(root());
  }
};

} // anonymous namespace

TEST_F(LlvmLibcDirentTest, SimpleOpenAndRead) {
  ::DIR *dir = LIBC_NAMESPACE::opendir(root());
  ASSERT_TRUE(dir != nullptr);
  // The file descriptors 0, 1 and 2 are reserved for standard streams.
  // So, the file descriptor for the newly opened directory should be
  // greater than 2.
  ASSERT_GT(LIBC_NAMESPACE::dirfd(dir), 2);

  struct ::dirent *file1 = nullptr, *file2 = nullptr, *dir1 = nullptr,
                  *dir2 = nullptr;
  while (true) {
    struct ::dirent *d = LIBC_NAMESPACE::readdir(dir);
    if (d == nullptr)
      break;
    if (string_view(&d->d_name[0]) == "file1.txt") {
      file1 = d;
      EXPECT_EQ(d->d_type, static_cast<decltype(d->d_type)>(DT_REG));
    }
    if (string_view(&d->d_name[0]) == "file2.txt") {
      file2 = d;
      EXPECT_EQ(d->d_type, static_cast<decltype(d->d_type)>(DT_REG));
    }
    if (string_view(&d->d_name[0]) == "dir1") {
      dir1 = d;
      EXPECT_EQ(d->d_type, static_cast<decltype(d->d_type)>(DT_DIR));
    }
    if (string_view(&d->d_name[0]) == "dir2") {
      dir2 = d;
      EXPECT_EQ(d->d_type, static_cast<decltype(d->d_type)>(DT_DIR));
    }
  }

  // Verify that we don't break out of the above loop in error.
  ASSERT_ERRNO_SUCCESS();

  ASSERT_TRUE(file1 != nullptr);
  ASSERT_TRUE(file2 != nullptr);
  ASSERT_TRUE(dir1 != nullptr);
  ASSERT_TRUE(dir2 != nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}

TEST_F(LlvmLibcDirentTest, OpenNonExistentDir) {
  ::DIR *dir = LIBC_NAMESPACE::opendir("___xyz123__.non_existent__");
  ASSERT_TRUE(dir == nullptr);
  ASSERT_ERRNO_EQ(ENOENT);
}

TEST_F(LlvmLibcDirentTest, OpenFile) {
  char path[512];
  join(path, "file1.txt");
  ::DIR *dir = LIBC_NAMESPACE::opendir(path);
  ASSERT_TRUE(dir == nullptr);
  ASSERT_ERRNO_EQ(ENOTDIR);
}

TEST_F(LlvmLibcDirentTest, ADirectoryRemovedWhileOpenIsSimplyEnded) {
  auto path = libc_make_test_file_path("dirent_removed");
  ASSERT_EQ(LIBC_NAMESPACE::mkdir(path, S_IRWXU), 0);

  ::DIR *dir = LIBC_NAMESPACE::opendir(path);
  ASSERT_TRUE(dir != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::rmdir(path), 0);

  // There is nothing more in a directory that is no longer there, which is
  // the end of it rather than something going wrong. A caller tells the two
  // apart by looking at errno, so this must leave it alone.
  libc_errno = 0;
  EXPECT_EQ(LIBC_NAMESPACE::readdir(dir),
            static_cast<struct dirent *>(nullptr));
  ASSERT_ERRNO_SUCCESS();
  // And asking again says the same thing.
  EXPECT_EQ(LIBC_NAMESPACE::readdir(dir),
            static_cast<struct dirent *>(nullptr));
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}
