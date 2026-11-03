//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the netgroup functions.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/netdb/endnetgrent.h"
#include "src/netdb/getnetgrent.h"
#include "src/netdb/getnetgrent_r.h"
#include "src/netdb/innetgr.h"
#include "src/netdb/netgroup_utils.h"
#include "src/netdb/setnetgrent.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNetgroupTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *FILE_NAME = "netgroup.test";
constexpr const char *CONTENTS = "# comment\n"
                                 "staff (alpha,andy,) (beta,-,example) \\\n"
                                 "      (gamma, jo ,)\n"
                                 "all staff guests\n"
                                 "guests (delta,,)\n"
                                 "loop loop (zeta,,)\n";

void write_file() {
  int fd =
      LIBC_NAMESPACE::open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents(CONTENTS);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  LIBC_NAMESPACE::netgroup::TESTONLY_set_netgroup_path(FILE_NAME);
}

void finish() {
  LIBC_NAMESPACE::netgroup::TESTONLY_reset_netgroup_path();
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILE_NAME), 0);
}

} // anonymous namespace

TEST_F(LlvmLibcNetgroupTest, ReadsTheTriplesOfAGroup) {
  write_file();
  ASSERT_EQ(LIBC_NAMESPACE::setnetgrent("staff"), 1);
  char *host, *user, *domain;
  ASSERT_EQ(LIBC_NAMESPACE::getnetgrent(&host, &user, &domain), 1);
  ASSERT_STREQ(host, "alpha");
  ASSERT_STREQ(user, "andy");
  ASSERT_TRUE(domain == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::getnetgrent(&host, &user, &domain), 1);
  ASSERT_STREQ(host, "beta");
  ASSERT_STREQ(user, "-");
  ASSERT_STREQ(domain, "example");
  ASSERT_EQ(LIBC_NAMESPACE::getnetgrent(&host, &user, &domain), 1);
  ASSERT_STREQ(user, "jo");
  ASSERT_EQ(LIBC_NAMESPACE::getnetgrent(&host, &user, &domain), 0);
  LIBC_NAMESPACE::endnetgrent();
  ASSERT_EQ(LIBC_NAMESPACE::setnetgrent("missing"), 0);
  finish();
}

TEST_F(LlvmLibcNetgroupTest, GroupsNamedByAGroupAreExpanded) {
  write_file();
  ASSERT_EQ(LIBC_NAMESPACE::setnetgrent("all"), 1);
  int count = 0;
  char *host, *user, *domain;
  while (LIBC_NAMESPACE::getnetgrent(&host, &user, &domain) == 1)
    ++count;
  ASSERT_EQ(count, 4);
  LIBC_NAMESPACE::endnetgrent();
  // A group which names itself ends rather than looping.
  ASSERT_EQ(LIBC_NAMESPACE::setnetgrent("loop"), 1);
  LIBC_NAMESPACE::endnetgrent();
  finish();
}

TEST_F(LlvmLibcNetgroupTest, ReentrantFormUsesTheCallersBuffer) {
  write_file();
  ASSERT_EQ(LIBC_NAMESPACE::setnetgrent("staff"), 1);
  char *host, *user, *domain;
  char buf[64];
  ASSERT_EQ(
      LIBC_NAMESPACE::getnetgrent_r(&host, &user, &domain, buf, sizeof(buf)),
      1);
  ASSERT_STREQ(host, "alpha");
  ASSERT_TRUE(host >= buf && host < buf + sizeof(buf));
  ASSERT_TRUE(domain == nullptr);
  // The second triple needs more than the three bytes given, and is not
  // lost by the failure.
  ASSERT_EQ(LIBC_NAMESPACE::getnetgrent_r(&host, &user, &domain, buf, 3), 0);
  ASSERT_ERRNO_EQ(ERANGE);
  ASSERT_EQ(
      LIBC_NAMESPACE::getnetgrent_r(&host, &user, &domain, buf, sizeof(buf)),
      1);
  ASSERT_STREQ(host, "beta");
  LIBC_NAMESPACE::endnetgrent();
  finish();
}

TEST_F(LlvmLibcNetgroupTest, InNetgrMatchesTriplesAndWildcards) {
  write_file();
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("staff", "alpha", "andy", nullptr), 1);
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("staff", "alpha", "jo", nullptr), 0);
  // A null argument matches any value, and so does an empty field.
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("staff", nullptr, "jo", nullptr), 1);
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("staff", "gamma", "jo", "anything"), 1);
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("all", "delta", nullptr, nullptr), 1);
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("all", "omega", nullptr, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::innetgr("missing", nullptr, nullptr, nullptr), 0);
  finish();
}
