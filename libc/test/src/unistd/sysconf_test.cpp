//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sysconf
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/unistd_macros.h"
#include "src/unistd/sysconf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSysconfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSysconfTest, PagesizeTest) {
  long pagesize = LIBC_NAMESPACE::sysconf(_SC_PAGESIZE);
  ASSERT_GT(pagesize, 0L);
}

TEST_F(LlvmLibcSysconfTest, NprocessorsConfTest) {
  long sysconf_count = LIBC_NAMESPACE::sysconf(_SC_NPROCESSORS_CONF);
  ASSERT_GT(sysconf_count, 0L);
}

TEST_F(LlvmLibcSysconfTest, NprocessorsOnlnTest) {
  long sysconf_count = LIBC_NAMESPACE::sysconf(_SC_NPROCESSORS_ONLN);
  ASSERT_GT(sysconf_count, 0L);
}

TEST_F(LlvmLibcSysconfTest, PosixVersionTest) {
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_VERSION), 202405L);
}

TEST_F(LlvmLibcSysconfTest, PosixThreadsConstantsTest) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREADS), Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ATTR_STACKADDR),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ATTR_STACKSIZE),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_CPUTIME), Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_PRIO_INHERIT), Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_PRIO_PROTECT), Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_PRIORITY_SCHEDULING),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_PROCESS_SHARED),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ROBUST_PRIO_INHERIT),
              Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ROBUST_PRIO_PROTECT),
              Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_SAFE_FUNCTIONS),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_SPORADIC_SERVER),
              Succeeds(-1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS),
              Succeeds<long>(PTHREAD_DESTRUCTOR_ITERATIONS));
}

TEST_F(LlvmLibcSysconfTest, ArgMaxTest) {
  long arg_max = LIBC_NAMESPACE::sysconf(_SC_ARG_MAX);
  ASSERT_GT(arg_max, 0L);
  ASSERT_GE(arg_max, 131072L);
}

TEST_F(LlvmLibcSysconfTest, OpenMaxTest) {
  long open_max = LIBC_NAMESPACE::sysconf(_SC_OPEN_MAX);
  if (open_max == -1)
    return;
  ASSERT_GT(open_max, 0L);
}

TEST_F(LlvmLibcSysconfTest, PhysPagesTest) {
  long phys_pages = LIBC_NAMESPACE::sysconf(_SC_PHYS_PAGES);
  ASSERT_GT(phys_pages, 0L);
}

TEST_F(LlvmLibcSysconfTest, KnownConstantValuesTest) {
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_CLK_TCK), 100L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_GETGR_R_SIZE_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_GETPW_R_SIZE_MAX), -1L);
}

TEST_F(LlvmLibcSysconfTest, InvalidNameTest) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(100000), Fails(EINVAL, -1L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(0x7fffffff), Fails(EINVAL, -1L));
}

TEST_F(LlvmLibcSysconfTest, LimitsAgreeWithLimitsHeader) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_NGROUPS_MAX),
              Succeeds<long>(NGROUPS_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_IOV_MAX), Succeeds<long>(IOV_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_HOST_NAME_MAX),
              Succeeds<long>(HOST_NAME_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_LOGIN_NAME_MAX),
              Succeeds<long>(LOGIN_NAME_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_TTY_NAME_MAX),
              Succeeds<long>(TTY_NAME_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_RE_DUP_MAX),
              Succeeds<long>(RE_DUP_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_LINE_MAX),
              Succeeds<long>(_POSIX2_LINE_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_ATEXIT_MAX),
              Succeeds<long>(_POSIX_ATEXIT_MAX));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_STACK_MIN),
              Succeeds<long>(PTHREAD_STACK_MIN));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_KEYS_MAX),
              Succeeds<long>(PTHREAD_KEYS_MAX));
}

TEST_F(LlvmLibcSysconfTest, OptionsAreReported) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_JOB_CONTROL),
              Succeeds<long>(_POSIX_JOB_CONTROL));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_SAVED_IDS),
              Succeeds<long>(_POSIX_SAVED_IDS));
}

TEST_F(LlvmLibcSysconfTest, ChildMaxFollowsTheProcessLimit) {
  long child_max = LIBC_NAMESPACE::sysconf(_SC_CHILD_MAX);
  // -1 spells no limit, and leaves errno alone.
  if (child_max != -1)
    ASSERT_GT(child_max, 0L);
}

TEST_F(LlvmLibcSysconfTest, AvailablePagesAreReported) {
  long pages = LIBC_NAMESPACE::sysconf(_SC_AVPHYS_PAGES);
  ASSERT_GE(pages, 0L);
}
