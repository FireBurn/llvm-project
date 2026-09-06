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

#include "src/unistd/sysconf.h"
#include "test/UnitTest/Test.h"

#include <limits.h>
#include <unistd.h>

TEST(LlvmLibcSysconfTest, PagesizeTest) {
  long pagesize = LIBC_NAMESPACE::sysconf(_SC_PAGESIZE);
  ASSERT_GT(pagesize, 0L);
}

TEST(LlvmLibcSysconfTest, NprocessorsConfTest) {
  long sysconf_count = LIBC_NAMESPACE::sysconf(_SC_NPROCESSORS_CONF);
  ASSERT_GT(sysconf_count, 0L);
}

TEST(LlvmLibcSysconfTest, NprocessorsOnlnTest) {
  long sysconf_count = LIBC_NAMESPACE::sysconf(_SC_NPROCESSORS_ONLN);
  ASSERT_GT(sysconf_count, 0L);
}

TEST(LlvmLibcSysconfTest, ThreadsTest) {
  long threads = LIBC_NAMESPACE::sysconf(_SC_THREADS);
  ASSERT_EQ(threads, _POSIX_THREADS);
}

TEST(LlvmLibcSysconfTest, ArgMaxTest) {
  long arg_max = LIBC_NAMESPACE::sysconf(_SC_ARG_MAX);
  ASSERT_GT(arg_max, 0L);
  ASSERT_GE(arg_max, 131072L);
}

TEST(LlvmLibcSysconfTest, OpenMaxTest) {
  long open_max = LIBC_NAMESPACE::sysconf(_SC_OPEN_MAX);
  if (open_max == -1)
    return;
  ASSERT_GT(open_max, 0L);
}

TEST(LlvmLibcSysconfTest, PhysPagesTest) {
  long phys_pages = LIBC_NAMESPACE::sysconf(_SC_PHYS_PAGES);
  ASSERT_GT(phys_pages, 0L);
}

TEST(LlvmLibcSysconfTest, ClkTckTest) {
  long clk_tck = LIBC_NAMESPACE::sysconf(_SC_CLK_TCK);
  ASSERT_EQ(clk_tck, 100L);
}

// Each option group reports the edition of POSIX it was taken from, which is
// what the matching macro in <unistd.h> says.
TEST(LlvmLibcSysconfTest, OptionGroupsTest) {
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_TIMERS), (long)_POSIX_TIMERS);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_MONOTONIC_CLOCK),
            (long)_POSIX_MONOTONIC_CLOCK);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_CLOCK_SELECTION),
            (long)_POSIX_CLOCK_SELECTION);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_SEMAPHORES), (long)_POSIX_SEMAPHORES);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_MAPPED_FILES),
            (long)_POSIX_MAPPED_FILES);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_MEMORY_PROTECTION),
            (long)_POSIX_MEMORY_PROTECTION);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_FSYNC), (long)_POSIX_FSYNC);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_SYNCHRONIZED_IO),
            (long)_POSIX_SYNCHRONIZED_IO);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_THREAD_SAFE_FUNCTIONS),
            (long)_POSIX_THREAD_SAFE_FUNCTIONS);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_SHELL), (long)_POSIX_SHELL);
}

TEST(LlvmLibcSysconfTest, VersionsTest) {
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_VERSION), (long)_POSIX2_VERSION);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_VERSION), (long)_XOPEN_VERSION);
}

// The limits which do not change report what <limits.h> states.
TEST(LlvmLibcSysconfTest, FixedLimitsTest) {
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS),
            (long)PTHREAD_DESTRUCTOR_ITERATIONS);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_SEM_VALUE_MAX), (long)SEM_VALUE_MAX);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_DELAYTIMER_MAX), (long)DELAYTIMER_MAX);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_MQ_PRIO_MAX), (long)MQ_PRIO_MAX);
}
