//===-- Unittests for copy_file_range -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/string/memcmp.h"
#include "src/unistd/close.h"
#include "src/unistd/copy_file_range.h"
#include "src/unistd/lseek.h"
#include "src/unistd/read.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCopyFileRangeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcCopyFileRangeTest, CopiesFromOneFileToAnother) {
  constexpr char TEXT[] = "copy_file_range";
  constexpr size_t SIZE = sizeof(TEXT) - 1;

  auto in_path = libc_make_test_file_path("copy_file_range_in.test");
  auto out_path = libc_make_test_file_path("copy_file_range_out.test");

  int in = LIBC_NAMESPACE::open(in_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(in, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(in, TEXT, SIZE), Succeeds<ssize_t>(SIZE));
  ASSERT_THAT(LIBC_NAMESPACE::lseek(in, 0, SEEK_SET), Succeeds<off_t>(0));

  int out = LIBC_NAMESPACE::open(out_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(out, 0);

  ssize_t copied =
      LIBC_NAMESPACE::copy_file_range(in, nullptr, out, nullptr, SIZE, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(copied, static_cast<ssize_t>(SIZE));

  // Both offsets moved, since neither was given separately.
  ASSERT_THAT(LIBC_NAMESPACE::lseek(out, 0, SEEK_SET), Succeeds<off_t>(0));
  char buf[SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::read(out, buf, SIZE), Succeeds<ssize_t>(SIZE));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, TEXT, SIZE), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(in), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(out), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(in_path), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(out_path), Succeeds(0));
}

// An offset given separately is read and written through rather than the
// descriptor's own, which is left where it was.
TEST_F(LlvmLibcCopyFileRangeTest, CopiesFromAStatedOffset) {
  constexpr char TEXT[] = "0123456789";
  constexpr size_t SIZE = sizeof(TEXT) - 1;

  auto in_path = libc_make_test_file_path("copy_file_range_off_in.test");
  auto out_path = libc_make_test_file_path("copy_file_range_off_out.test");

  int in = LIBC_NAMESPACE::open(in_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(in, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(in, TEXT, SIZE), Succeeds<ssize_t>(SIZE));

  int out = LIBC_NAMESPACE::open(out_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(out, 0);

  off_t in_off = 4;
  off_t out_off = 0;
  ssize_t copied =
      LIBC_NAMESPACE::copy_file_range(in, &in_off, out, &out_off, 3, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(copied, static_cast<ssize_t>(3));
  ASSERT_EQ(in_off, static_cast<off_t>(7));
  ASSERT_EQ(out_off, static_cast<off_t>(3));

  // The descriptor's own offset was not moved.
  ASSERT_THAT(LIBC_NAMESPACE::lseek(in, 0, SEEK_CUR), Succeeds<off_t>(SIZE));

  ASSERT_THAT(LIBC_NAMESPACE::lseek(out, 0, SEEK_SET), Succeeds<off_t>(0));
  char buf[3];
  ASSERT_THAT(LIBC_NAMESPACE::read(out, buf, 3), Succeeds<ssize_t>(3));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, "456", 3), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(in), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(out), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(in_path), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(out_path), Succeeds(0));
}

TEST_F(LlvmLibcCopyFileRangeTest, DescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::copy_file_range(-1, nullptr, -1, nullptr, 1, 0),
              Fails<ssize_t>(EBADF));
}
