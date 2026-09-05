//===-- Unittests for pthread_setaffinity_np and its pair -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/cpu_set_t.h"
#include "src/pthread/pthread_getaffinity_np.h"
#include "src/pthread/pthread_self.h"
#include "src/pthread/pthread_setaffinity_np.h"
#include "src/sched/sched_getcpucount.h"
#include "src/sched/sched_getcpuisset.h"
#include "src/sched/sched_setcpuset.h"
#include "src/string/memset.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

TEST(LlvmLibcPthreadAffinityTest, ReportsWhereItMayRun) {
  cpu_set_t set;
  LIBC_NAMESPACE::memset(&set, 0, sizeof(set));
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getaffinity_np(
                LIBC_NAMESPACE::pthread_self(), sizeof(set), &set),
            0);
  // The thread has to be allowed to run somewhere.
  EXPECT_GT(int(LIBC_NAMESPACE::__sched_getcpucount(sizeof(set), &set)), 0);
}

TEST(LlvmLibcPthreadAffinityTest, NarrowsToOneProcessorAndBack) {
  cpu_set_t original;
  LIBC_NAMESPACE::memset(&original, 0, sizeof(original));
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getaffinity_np(
                LIBC_NAMESPACE::pthread_self(), sizeof(original), &original),
            0);

  cpu_set_t one;
  LIBC_NAMESPACE::memset(&one, 0, sizeof(one));
  LIBC_NAMESPACE::__sched_setcpuset(0, sizeof(one), &one);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setaffinity_np(
                LIBC_NAMESPACE::pthread_self(), sizeof(one), &one),
            0);

  cpu_set_t now;
  LIBC_NAMESPACE::memset(&now, 0, sizeof(now));
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getaffinity_np(
                LIBC_NAMESPACE::pthread_self(), sizeof(now), &now),
            0);
  EXPECT_EQ(int(LIBC_NAMESPACE::__sched_getcpucount(sizeof(now), &now)), 1);
  EXPECT_NE(LIBC_NAMESPACE::__sched_getcpuisset(0, sizeof(now), &now), 0);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_setaffinity_np(
                LIBC_NAMESPACE::pthread_self(), sizeof(original), &original),
            0);
}

TEST(LlvmLibcPthreadAffinityTest, ReportsTheErrorRatherThanSettingErrno) {
  cpu_set_t set;
  // A set the kernel will not accept, so the error comes back as the result.
  EXPECT_EQ(LIBC_NAMESPACE::pthread_getaffinity_np(
                LIBC_NAMESPACE::pthread_self(), 1, &set),
            EINVAL);
}
