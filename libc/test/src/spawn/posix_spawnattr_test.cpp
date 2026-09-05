//===-- Unittests for posix_spawnattr_t -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/spawn_macros.h"
#include "hdr/types/posix_spawnattr_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_sched_param.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigismember.h"
#include "src/spawn/posix_spawnattr_destroy.h"
#include "src/spawn/posix_spawnattr_getflags.h"
#include "src/spawn/posix_spawnattr_getpgroup.h"
#include "src/spawn/posix_spawnattr_getschedparam.h"
#include "src/spawn/posix_spawnattr_getschedpolicy.h"
#include "src/spawn/posix_spawnattr_getsigdefault.h"
#include "src/spawn/posix_spawnattr_getsigmask.h"
#include "src/spawn/posix_spawnattr_init.h"
#include "src/spawn/posix_spawnattr_setflags.h"
#include "src/spawn/posix_spawnattr_setpgroup.h"
#include "src/spawn/posix_spawnattr_setschedparam.h"
#include "src/spawn/posix_spawnattr_setschedpolicy.h"
#include "src/spawn/posix_spawnattr_setsigdefault.h"
#include "src/spawn/posix_spawnattr_setsigmask.h"
#include "test/UnitTest/Test.h"

// These report the error rather than setting errno, so the return value is
// what each check looks at.

TEST(LlvmLibcPosixSpawnattrTest, InitClearsEverything) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  short flags = -1;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getflags(&attr, &flags), 0);
  ASSERT_EQ(flags, static_cast<short>(0));

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}

TEST(LlvmLibcPosixSpawnattrTest, FlagsRoundTrip) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  constexpr short SET = POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_SETSIGMASK;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setflags(&attr, SET), 0);

  short flags = 0;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getflags(&attr, &flags), 0);
  ASSERT_EQ(flags, SET);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}

TEST(LlvmLibcPosixSpawnattrTest, PgroupRoundTrip) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setpgroup(&attr, 42), 0);
  pid_t pgroup = 0;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getpgroup(&attr, &pgroup), 0);
  ASSERT_EQ(pgroup, static_cast<pid_t>(42));

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}

TEST(LlvmLibcPosixSpawnattrTest, SchedulerRoundTrip) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setschedpolicy(&attr, 2), 0);
  int policy = 0;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedpolicy(&attr, &policy), 0);
  ASSERT_EQ(policy, 2);

  struct sched_param param;
  param.sched_priority = 7;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setschedparam(&attr, &param), 0);

  struct sched_param read;
  read.sched_priority = 0;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedparam(&attr, &read), 0);
  ASSERT_EQ(read.sched_priority, 7);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}

TEST(LlvmLibcPosixSpawnattrTest, SignalSetsRoundTrip) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  sigset_t set;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&set), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&set, SIGUSR1), 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setsigmask(&attr, &set), 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setsigdefault(&attr, &set), 0);

  sigset_t read;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigmask(&attr, &read), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&read, SIGUSR1), 1);
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&read, SIGUSR2), 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigdefault(&attr, &read), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&read, SIGUSR1), 1);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}

TEST(LlvmLibcPosixSpawnattrTest, NullArgumentsAreRefused) {
  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);

  short flags = 0;
  pid_t pgroup = 0;
  int policy = 0;
  struct sched_param param;
  sigset_t set;

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getflags(nullptr, &flags), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getflags(&attr, nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setflags(nullptr, 0), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getpgroup(nullptr, &pgroup),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getpgroup(&attr, nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setpgroup(nullptr, 0), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedpolicy(nullptr, &policy),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedpolicy(&attr, nullptr),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setschedpolicy(nullptr, 0), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedparam(nullptr, &param),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getschedparam(&attr, nullptr),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setschedparam(&attr, nullptr),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigmask(nullptr, &set), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigmask(&attr, nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setsigmask(&attr, nullptr), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigdefault(nullptr, &set),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_getsigdefault(&attr, nullptr),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setsigdefault(&attr, nullptr),
            EINVAL);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
}
