//===-- Unittests for the glibc compatibility symbols ---------------------===//
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
#include "src/__support/libc_errno.h"
#include "src/compat/__errno_location.h"
#include "src/compat/__xpg_strerror_r.h"
#include "src/compat/lfs64.h"
#include "src/compat/syscall.h"
#include "src/dirent/closedir.h"
#include "src/dirent/opendir.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fputs.h"
#include "src/string/strcmp.h"
#include "src/unistd/close.h"
#include "src/unistd/getpid.h"
#include "src/unistd/write.h"
#include "test/UnitTest/Test.h"

#include <sys/syscall.h>

// The headers included above state the sixty four names as macros onto the
// calls they name. This reaches the symbols themselves, which is what the
// tests below are for.
#include "src/compat/lfs64_undef.h"

// unistd.h states syscall as a macro that dispatches to a fixed seven
// arguments. The function of the same name is what anything already built
// reaches for, so the macro is set aside to get at it.
#undef syscall

TEST(LlvmLibcCompat, ErrnoLocationIsWhereErrnoIs) {
  int *where = LIBC_NAMESPACE::__errno_location();
  ASSERT_NE(where, static_cast<int *>(nullptr));
  // The same place every time it is asked, and the place errno itself is.
  ASSERT_EQ(where, LIBC_NAMESPACE::__errno_location());
  LIBC_NAMESPACE::libc_errno = 0;
  *where = EINVAL;
  ASSERT_EQ(static_cast<int>(LIBC_NAMESPACE::libc_errno), EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST(LlvmLibcCompat, SyscallByNumber) {
  // Bypasses the macro of the same name, which is what anything already built
  // does too.
  ASSERT_EQ(static_cast<int>(LIBC_NAMESPACE::syscall(SYS_getpid)),
            LIBC_NAMESPACE::getpid());
}

TEST(LlvmLibcCompat, SyscallReportsFailureTheWayALibcDoes) {
  LIBC_NAMESPACE::libc_errno = 0;
  // The kernel hands back the negated error number; a libc answers with minus
  // one and puts it in errno.
  ASSERT_EQ(LIBC_NAMESPACE::syscall(SYS_close, 0x7FFFFFFF), -1L);
  ASSERT_EQ(static_cast<int>(LIBC_NAMESPACE::libc_errno), EBADF);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST(LlvmLibcCompat, XpgStrerrorR) {
  char buffer[64];
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EINVAL, buffer, sizeof(buffer)),
            0);
  ASSERT_STREQ(buffer, "Invalid argument");
}

TEST(LlvmLibcCompat, XpgStrerrorRSaysWhenItDoesNotFit) {
  char buffer[4];
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EINVAL, buffer, sizeof(buffer)),
            ERANGE);
  // What did fit is still there, and still ends.
  ASSERT_EQ(buffer[3], '\0');
}

TEST(LlvmLibcCompat, XpgStrerrorRSaysWhenTheNumberNamesNoError) {
  char buffer[64];
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(9999, buffer, sizeof(buffer)),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EINVAL, nullptr, 64), EINVAL);
}

TEST(LlvmLibcCompat, TheSixtyFourNamesAreTheOrdinaryCalls) {
  constexpr const char *PATH = "compat_lfs64.test";
  int fd = LIBC_NAMESPACE::open64(PATH, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, "0123456789", 10),
            static_cast<ssize_t>(10));

  struct stat from_fd;
  ASSERT_EQ(LIBC_NAMESPACE::fstat64(fd, &from_fd), 0);
  ASSERT_EQ(from_fd.st_size, static_cast<off_t>(10));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  struct stat from_path;
  ASSERT_EQ(LIBC_NAMESPACE::stat64(PATH, &from_path), 0);
  ASSERT_EQ(from_path.st_size, static_cast<off_t>(10));
  ASSERT_EQ(from_path.st_ino, from_fd.st_ino);

  fd = LIBC_NAMESPACE::open64(PATH, O_RDONLY);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::lseek64(fd, 4, SEEK_SET), static_cast<off_t>(4));
  ASSERT_EQ(LIBC_NAMESPACE::lseek64(fd, 0, SEEK_END), static_cast<off_t>(10));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

TEST(LlvmLibcCompat, TheSixtyFourNamesReachTheSameDirectoryCalls) {
  DIR *dir = LIBC_NAMESPACE::opendir(".");
  ASSERT_NE(dir, static_cast<DIR *>(nullptr));
  // Every directory has itself in it, so there is always something to read.
  ASSERT_NE(LIBC_NAMESPACE::readdir64(dir),
            static_cast<struct dirent *>(nullptr));
  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}

TEST(LlvmLibcCompat, TheSixtyFourNamesReachTheSameStreamCalls) {
  constexpr const char *PATH = "compat_lfs64_stream.test";
  FILE *stream = LIBC_NAMESPACE::fopen64(PATH, "w");
  ASSERT_NE(stream, static_cast<FILE *>(nullptr));
  ASSERT_GE(LIBC_NAMESPACE::fputs("0123456789", stream), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);

  stream = LIBC_NAMESPACE::fopen64(PATH, "r");
  ASSERT_NE(stream, static_cast<FILE *>(nullptr));
  ASSERT_EQ(LIBC_NAMESPACE::fseeko64(stream, 4, SEEK_SET), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ftello64(stream), static_cast<off_t>(4));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
}
