//===-- Unittests for sendfile --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/sys/sendfile/sendfile.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSendfileTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
namespace cpp = LIBC_NAMESPACE::cpp;

TEST_F(LlvmLibcSendfileTest, CreateAndTransfer) {
  // The test strategy is to
  //   1. Create a temporary file with known data.
  //   2. Use sendfile to copy it to another file.
  //   3. Make sure that the data was actually copied.
  //   4. Clean up the temporary files.
  constexpr const char *IN_FILE = "testdata/sendfile_in.test";
  constexpr const char *OUT_FILE = "testdata/sendfile_out.test";
  const char IN_DATA[] = "sendfile test";
  constexpr ssize_t IN_SIZE = ssize_t(sizeof(IN_DATA));

  int in_fd = LIBC_NAMESPACE::open(IN_FILE, O_CREAT | O_WRONLY, S_IRWXU);
  ASSERT_GT(in_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(LIBC_NAMESPACE::write(in_fd, IN_DATA, IN_SIZE), IN_SIZE);
  ASSERT_THAT(LIBC_NAMESPACE::close(in_fd), Succeeds(0));

  in_fd = LIBC_NAMESPACE::open(IN_FILE, O_RDONLY);
  ASSERT_GT(in_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  int out_fd = LIBC_NAMESPACE::open(OUT_FILE, O_CREAT | O_WRONLY, S_IRWXU);
  ASSERT_GT(out_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  // The destination comes first, as it does for the syscall.
  ssize_t size = LIBC_NAMESPACE::sendfile(out_fd, in_fd, nullptr, IN_SIZE);
  ASSERT_EQ(size, IN_SIZE);
  ASSERT_THAT(LIBC_NAMESPACE::close(in_fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(out_fd), Succeeds(0));

  out_fd = LIBC_NAMESPACE::open(OUT_FILE, O_RDONLY);
  ASSERT_GT(out_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  char buf[IN_SIZE];
  ASSERT_EQ(IN_SIZE, LIBC_NAMESPACE::read(out_fd, buf, IN_SIZE));
  ASSERT_EQ(cpp::string_view(buf), cpp::string_view(IN_DATA));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(IN_FILE), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(OUT_FILE), Succeeds(0));
}

TEST_F(LlvmLibcSendfileTest, ReadsFromTheSecondAndWritesToTheFirst) {
  // Which way round the descriptors go is what this is about, so it is
  // checked by giving one of them no permission to do what it would need.
  constexpr const char *IN_FILE = "testdata/sendfile_direction.test";
  const char IN_DATA[] = "direction";
  constexpr ssize_t IN_SIZE = ssize_t(sizeof(IN_DATA));

  int fd = LIBC_NAMESPACE::open(IN_FILE, O_CREAT | O_WRONLY, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, IN_DATA, IN_SIZE), IN_SIZE);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  int readable = LIBC_NAMESPACE::open(IN_FILE, O_RDONLY);
  ASSERT_GT(readable, 0);

  // Reading from something opened only for reading and writing to it as well
  // cannot work, and which error comes back says which way round they went.
  ASSERT_EQ(LIBC_NAMESPACE::sendfile(readable, readable, nullptr, IN_SIZE),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EBADF);
  libc_errno = 0;

  ASSERT_THAT(LIBC_NAMESPACE::close(readable), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(IN_FILE), Succeeds(0));
}
