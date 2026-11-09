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
#include "hdr/signal_macros.h"
#include "hdr/sys_resource_macros.h"
#include "hdr/types/struct_rlimit.h"
#include "hdr/unistd_macros.h"
#include "src/sys/resource/getrlimit.h"
#include "src/unistd/sysconf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"

#include <limits.h>
#include <unistd.h>

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

TEST_F(LlvmLibcSysconfTest, PosixTimeoutsTest) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_TIMEOUTS),
              Succeeds(static_cast<long>(_POSIX_TIMEOUTS)));
}

TEST_F(LlvmLibcSysconfTest, PosixThreadsConstantsTest) {
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREADS), Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ATTR_STACKADDR),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_ATTR_STACKSIZE),
              Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_CPUTIME), Succeeds(202405L));
  EXPECT_THAT(LIBC_NAMESPACE::sysconf(_SC_THREAD_PRIO_INHERIT),
              Succeeds(202405L));
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

// Each option group reports the edition of POSIX it was taken from, which is
// what the matching macro in <unistd.h> says.
TEST_F(LlvmLibcSysconfTest, OptionGroupsTest) {
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
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_UNIX), 1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_CRYPT), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_ENH_I18N), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_SHM), 1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_LEGACY), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_REALTIME), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_REALTIME_THREADS), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_STREAMS), -1L);
}

// The limits which do not change report what <limits.h> states.
TEST(LlvmLibcSysconfTest, FixedLimitsTest) {
  // Obsolete or unsupported: -1 with errno left alone.
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_PASS_MAX), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_PRIORITIZED_IO), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XOPEN_XCU_VERSION), -1L);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_XBS5_LP64_OFF64),
            LIBC_NAMESPACE::sysconf(_SC_V7_LP64_OFF64));
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS),
            (long)PTHREAD_DESTRUCTOR_ITERATIONS);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_SEM_VALUE_MAX), (long)SEM_VALUE_MAX);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_DELAYTIMER_MAX), (long)DELAYTIMER_MAX);
  ASSERT_EQ(LIBC_NAMESPACE::sysconf(_SC_MQ_PRIO_MAX), (long)MQ_PRIO_MAX);
}

TEST_F(LlvmLibcSysconfTest, OptionsWhoseInterfacesArePresent) {
  // Each is declared in <unistd.h>, and sysconf has to give the same answer.
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_REALTIME_SIGNALS),
            long(_POSIX_REALTIME_SIGNALS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_PRIORITY_SCHEDULING),
            long(_POSIX_PRIORITY_SCHEDULING));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_MEMLOCK), long(_POSIX_MEMLOCK));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_MEMLOCK_RANGE),
            long(_POSIX_MEMLOCK_RANGE));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_MESSAGE_PASSING),
            long(_POSIX_MESSAGE_PASSING));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SHARED_MEMORY_OBJECTS),
            long(_POSIX_SHARED_MEMORY_OBJECTS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_ADVISORY_INFO),
            long(_POSIX_ADVISORY_INFO));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_CPUTIME), long(_POSIX_CPUTIME));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_READER_WRITER_LOCKS),
            long(_POSIX_READER_WRITER_LOCKS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SPIN_LOCKS), long(_POSIX_SPIN_LOCKS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_REGEXP), long(_POSIX_REGEXP));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SPAWN), long(_POSIX_SPAWN));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_IPV6), long(_POSIX_IPV6));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_RAW_SOCKETS), long(_POSIX_RAW_SOCKETS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_THREAD_CPUTIME),
            long(_POSIX_THREAD_CPUTIME));
  EXPECT_GT(long(_POSIX_ADVISORY_INFO), 0L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_BARRIERS), long(_POSIX_BARRIERS));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_C_BIND), long(_POSIX2_C_BIND));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_C_VERSION), long(_POSIX2_C_VERSION));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_C_DEV), long(_POSIX2_C_DEV));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_CHAR_TERM), long(_POSIX2_CHAR_TERM));
}

TEST(LlvmLibcSysconfTest, UnsupportedOptionsAreMinusOne) {
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_FORT_DEV), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_FORT_RUN), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_SW_DEV), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_LOCALEDEF), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_UPE), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS_ACCOUNTING), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS_LOCATE), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS_MESSAGE), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS_TRACK), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_2_PBS_CHECKPOINT), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_ASYNCHRONOUS_IO), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SPORADIC_SERVER), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_EVENT_FILTER), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_INHERIT), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_LOG), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TYPED_MEMORY_OBJECTS), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SS_REPL_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_EVENT_NAME_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_NAME_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_SYS_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TRACE_USER_EVENT_MAX), -1L);
}

TEST(LlvmLibcSysconfTest, ProgrammingEnvironments) {
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_V7_ILP32_OFF32), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_XBS5_ILP32_OFF32), -1L);
  if (sizeof(long) == 8 && sizeof(void *) == 8) {
    EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_V7_LP64_OFF64), 1L);
    EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_V7_LPBIG_OFFBIG), 1L);
    EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_V7_ILP32_OFFBIG), -1L);
  }
}

TEST(LlvmLibcSysconfTest, LimitsTheStandardLeavesToTheSystem) {
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_AIO_LISTIO_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_AIO_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_AIO_PRIO_DELTA_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_MQ_OPEN_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SEM_NSEMS_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_TIMER_MAX), -1L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_RTSIG_MAX),
            static_cast<long>(SIGRTMAX - SIGRTMIN + 1));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_BC_BASE_MAX), 99L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_BC_DIM_MAX), 2048L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_BC_SCALE_MAX), 99L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_BC_STRING_MAX), 1000L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_COLL_WEIGHTS_MAX), 255L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_EXPR_NEST_MAX), 32L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_UIO_MAXIOV),
            LIBC_NAMESPACE::sysconf(_SC_IOV_MAX));
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_LEVEL1_DCACHE_SIZE), 0L);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_LEVEL4_CACHE_LINESIZE), 0L);
}

TEST(LlvmLibcSysconfTest, SigqueueMaxIsTheSignalLimit) {
  struct rlimit limits;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_SIGPENDING, &limits), 0);
  long expected = limits.rlim_cur == RLIM_INFINITY
                      ? -1L
                      : static_cast<long>(limits.rlim_cur);
  EXPECT_EQ(LIBC_NAMESPACE::sysconf(_SC_SIGQUEUE_MAX), expected);
}
