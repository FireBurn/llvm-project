//===-- Unittests for pthread_mutexattr_t ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/pthread/pthread_mutexattr_destroy.h"
#include "src/pthread/pthread_mutexattr_getprotocol.h"
#include "src/pthread/pthread_mutexattr_getpshared.h"
#include "src/pthread/pthread_mutexattr_getrobust.h"
#include "src/pthread/pthread_mutexattr_gettype.h"
#include "src/pthread/pthread_mutexattr_init.h"
#include "src/pthread/pthread_mutexattr_setprotocol.h"
#include "src/pthread/pthread_mutexattr_setpshared.h"
#include "src/pthread/pthread_mutexattr_setrobust.h"
#include "src/pthread/pthread_mutexattr_settype.h"
#include "test/UnitTest/Test.h"
#include <pthread.h>

TEST(LlvmLibcPThreadMutexAttrTest, InitAndDestroy) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_destroy(&attr), 0);
}

TEST(LlvmLibcPThreadMutexAttrTest, SetAndGetType) {
  int type;
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_gettype(&attr, &type), 0);
  ASSERT_EQ(type, int(PTHREAD_MUTEX_DEFAULT));

  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_gettype(&attr, &type), 0);
  ASSERT_EQ(type, int(PTHREAD_MUTEX_RECURSIVE));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_settype(&attr,
                                                      PTHREAD_MUTEX_ERRORCHECK),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_gettype(&attr, &type), 0);
  ASSERT_EQ(type, int(PTHREAD_MUTEX_ERRORCHECK));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_settype(&attr, 0xBAD), EINVAL);
}

TEST(LlvmLibcPThreadMutexAttrTest, SetAndGetRobust) {
  int robust;
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getrobust(&attr, &robust), 0);
  ASSERT_EQ(robust, int(PTHREAD_MUTEX_STALLED));

  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getrobust(&attr, &robust), 0);
  ASSERT_EQ(robust, int(PTHREAD_MUTEX_ROBUST));

  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_STALLED),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getrobust(&attr, &robust), 0);
  ASSERT_EQ(robust, int(PTHREAD_MUTEX_STALLED));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setrobust(&attr, 0xBAD), EINVAL);
}

TEST(LlvmLibcPThreadMutexAttrTest, SetAndGetPShared) {
  int pshared;
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, int(PTHREAD_PROCESS_PRIVATE));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setpshared(
                &attr, PTHREAD_PROCESS_SHARED),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, int(PTHREAD_PROCESS_SHARED));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setpshared(
                &attr, PTHREAD_PROCESS_PRIVATE),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, int(PTHREAD_PROCESS_PRIVATE));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setpshared(&attr, 0xBAD), EINVAL);
}

TEST(LlvmLibcPThreadMutexAttrTest, SetAndGetProtocol) {
  int protocol;
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, int(PTHREAD_PRIO_NONE));

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr,
                                                          PTHREAD_PRIO_INHERIT),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, int(PTHREAD_PRIO_INHERIT));

  // A ceiling is not supported, and asking for one leaves the attribute as
  // it was.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr,
                                                          PTHREAD_PRIO_PROTECT),
            ENOTSUP);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, int(PTHREAD_PRIO_INHERIT));
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr, 0xBAD),
            EINVAL);

  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_NONE),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, int(PTHREAD_PRIO_NONE));
}
