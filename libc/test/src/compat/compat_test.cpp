//===-- Unittests for the glibc compatibility symbols ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/compat/__errno_location.h"
#include "src/compat/__xpg_strerror_r.h"
#include "src/compat/gnu_get_libc_release.h"
#include "src/compat/gnu_get_libc_version.h"
#include "src/string/strlen.h"
#include "test/UnitTest/Test.h"

// Programs ask the C library what version it is and refuse to run against
// one older than they were built for. The answer has to parse as a version
// and be recent enough to satisfy them.
TEST(LlvmLibcCompatTest, ReportsAVersion) {
  const char *version = LIBC_NAMESPACE::gnu_get_libc_version();
  ASSERT_FALSE(version == nullptr);
  ASSERT_GT(LIBC_NAMESPACE::strlen(version), size_t(0));

  // Of the form major.minor, and nothing else.
  int major = 0;
  const char *p = version;
  ASSERT_TRUE(*p >= '0' && *p <= '9');
  while (*p >= '0' && *p <= '9')
    major = major * 10 + (*p++ - '0');
  ASSERT_EQ(*p, '.');
  ++p;
  ASSERT_TRUE(*p >= '0' && *p <= '9');
  while (*p >= '0' && *p <= '9')
    ++p;
  ASSERT_EQ(*p, '\0');
  ASSERT_GE(major, 2);
}

// The release is the word glibc uses for how the version was made, and
// "stable" is the only one anything looks for.
TEST(LlvmLibcCompatTest, ReportsARelease) {
  const char *release = LIBC_NAMESPACE::gnu_get_libc_release();
  ASSERT_FALSE(release == nullptr);
  ASSERT_STREQ(release, "stable");
}

// __errno_location is where errno is. Anything already built against a
// library which had it reaches errno that way, so it has to name the same
// place this library's own errno does.
TEST(LlvmLibcCompatTest, ErrnoLocationNamesErrno) {
  int *location = LIBC_NAMESPACE::__errno_location();
  ASSERT_FALSE(location == nullptr);

  LIBC_NAMESPACE::libc_errno = EDOM;
  ASSERT_EQ(*location, EDOM);

  *location = ERANGE;
  ASSERT_EQ(static_cast<int>(LIBC_NAMESPACE::libc_errno), ERANGE);

  LIBC_NAMESPACE::libc_errno = 0;
}

// __xpg_strerror_r is the POSIX form of strerror_r: it writes into the
// caller's buffer and reports what went wrong rather than returning a
// pointer to a message.
TEST(LlvmLibcCompatTest, XpgStrerrorRWritesIntoTheCallersBuffer) {
  char buf[64] = {};
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EDOM, buf, sizeof(buf)), 0);
  ASSERT_GT(LIBC_NAMESPACE::strlen(buf), size_t(0));

  // A number that names no error still gets a message, and says so.
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(-1, buf, sizeof(buf)), EINVAL);

  // A buffer too small for the message is filled as far as it goes and
  // reported, rather than written past.
  char small[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EDOM, small, sizeof(small)),
            ERANGE);
  ASSERT_EQ(small[sizeof(small) - 1], '\0');

  ASSERT_EQ(LIBC_NAMESPACE::__xpg_strerror_r(EDOM, nullptr, sizeof(buf)),
            EINVAL);
}
