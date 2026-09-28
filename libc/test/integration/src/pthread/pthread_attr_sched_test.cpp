//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for the scheduling attributes pthread_create applies.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sched_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/pthread_t.h"
#include "hdr/types/struct_sched_param.h"
#include "src/pthread/pthread_attr_init.h"
#include "src/pthread/pthread_attr_setinheritsched.h"
#include "src/pthread/pthread_attr_setschedparam.h"
#include "src/pthread/pthread_attr_setschedpolicy.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/sched/sched_getscheduler.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static void *report_policy(void *) {
  return reinterpret_cast<void *>(
      static_cast<intptr_t>(LIBC_NAMESPACE::sched_getscheduler(0)));
}

static int policy_of_new_thread(const pthread_attr_t *attr) {
  pthread_t th;
  if (LIBC_NAMESPACE::pthread_create(&th, attr, report_policy, nullptr) != 0)
    return -1;
  void *retval = nullptr;
  if (LIBC_NAMESPACE::pthread_join(th, &retval) != 0)
    return -1;
  return static_cast<int>(reinterpret_cast<intptr_t>(retval));
}

TEST_MAIN() {
  int creator = LIBC_NAMESPACE::sched_getscheduler(0);
  ASSERT_TRUE(creator >= 0);

  // A thread inherits the policy of its creator by default, with or without
  // attributes.
  ASSERT_EQ(policy_of_new_thread(nullptr), creator);
  pthread_attr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_init(&attr), 0);
  ASSERT_EQ(policy_of_new_thread(&attr), creator);

  // Attributes which name a policy are ignored while inheriting.
  struct sched_param param = {};
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedpolicy(&attr, SCHED_BATCH), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedparam(&attr, &param), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setinheritsched(
                &attr, PTHREAD_INHERIT_SCHED),
            0);
  ASSERT_EQ(policy_of_new_thread(&attr), creator);

  // Where they ask for the explicit form, the thread gets that policy. Batch
  // needs no privilege, unlike the real time policies.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setinheritsched(
                &attr, PTHREAD_EXPLICIT_SCHED),
            0);
  ASSERT_EQ(policy_of_new_thread(&attr), SCHED_BATCH);

  // A policy the process may not use is refused by pthread_create, and no
  // thread is left running. Repeated, so that a thread which was left behind
  // or which ran before the refusal would show.
  struct sched_param realtime = {};
  realtime.sched_priority = 1;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedpolicy(&attr, SCHED_FIFO), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedparam(&attr, &realtime), 0);
  for (int i = 0; i < 20; ++i) {
    pthread_t th;
    int error =
        LIBC_NAMESPACE::pthread_create(&th, &attr, report_policy, nullptr);
    if (error == 0) {
      // Permitted, as it is for the superuser.
      void *retval = nullptr;
      ASSERT_EQ(LIBC_NAMESPACE::pthread_join(th, &retval), 0);
      ASSERT_EQ(static_cast<int>(reinterpret_cast<intptr_t>(retval)),
                SCHED_FIFO);
    } else {
      ASSERT_EQ(error, EPERM);
    }
  }
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedpolicy(&attr, SCHED_BATCH), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setschedparam(&attr, &param), 0);

  // And the creator is left as it was.
  ASSERT_EQ(LIBC_NAMESPACE::sched_getscheduler(0), creator);
  return 0;
}
