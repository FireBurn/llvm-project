//===-- Unittests for getdents64 ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/types/struct_dirent.h"
#include "src/dirent/getdents64.h"
#include "src/fcntl/open.h"
#include "src/string/strcmp.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetdents64Test = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// getdents64 hands back the directory as the kernel writes it: entries of
// varying length laid end to end, each saying how far the next one is.
TEST_F(LlvmLibcGetdents64Test, ReadsTheEntriesOfADirectory) {
  int fd = LIBC_NAMESPACE::open(".", O_RDONLY | O_DIRECTORY);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  alignas(struct dirent) char buffer[4096];
  ssize_t read = LIBC_NAMESPACE::getdents64(fd, buffer, sizeof(buffer));
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(read, static_cast<ssize_t>(0));

  bool found_dot = false;
  bool found_dotdot = false;
  ssize_t offset = 0;
  while (offset < read) {
    auto *entry = reinterpret_cast<struct dirent *>(buffer + offset);
    ASSERT_GT(entry->d_reclen, static_cast<unsigned short>(0));
    if (LIBC_NAMESPACE::strcmp(entry->d_name, ".") == 0)
      found_dot = true;
    if (LIBC_NAMESPACE::strcmp(entry->d_name, "..") == 0)
      found_dotdot = true;
    offset += entry->d_reclen;
  }
  // The entries fill the answer exactly, with nothing left over.
  ASSERT_EQ(offset, read);
  ASSERT_TRUE(found_dot);
  ASSERT_TRUE(found_dotdot);

  // Reading on to the end reports that there is nothing more.
  ssize_t rest = 0;
  while ((rest = LIBC_NAMESPACE::getdents64(fd, buffer, sizeof(buffer))) > 0)
    ;
  ASSERT_EQ(rest, static_cast<ssize_t>(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcGetdents64Test, DescriptorThatIsNotOne) {
  char buffer[64];
  ASSERT_THAT(LIBC_NAMESPACE::getdents64(-1, buffer, sizeof(buffer)),
              Fails<ssize_t>(EBADF));
}

// A descriptor that is not a directory is not one to read entries from.
TEST_F(LlvmLibcGetdents64Test, DescriptorThatIsNotADirectory) {
  int fd = LIBC_NAMESPACE::open("/dev/null", O_RDONLY);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  char buffer[64];
  ASSERT_THAT(LIBC_NAMESPACE::getdents64(fd, buffer, sizeof(buffer)),
              Fails<ssize_t>(ENOTDIR));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}
