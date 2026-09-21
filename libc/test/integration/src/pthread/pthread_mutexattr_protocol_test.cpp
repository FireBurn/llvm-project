//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for pthread_mutexattr_setprotocol and getprotocol.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/pthread/pthread_mutexattr_destroy.h"
#include "src/pthread/pthread_mutexattr_getprotocol.h"
#include "src/pthread/pthread_mutexattr_init.h"
#include "src/pthread/pthread_mutexattr_setprotocol.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

TEST_MAIN() {
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);

  int protocol = -1;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, PTHREAD_PRIO_NONE);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(
                &attr, PTHREAD_PRIO_INHERIT),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, PTHREAD_PRIO_INHERIT);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr,
                                                          PTHREAD_PRIO_NONE),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, PTHREAD_PRIO_NONE);

  // Anything else is refused and leaves the setting alone.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setprotocol(&attr, 99), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_getprotocol(&attr, &protocol), 0);
  ASSERT_EQ(protocol, PTHREAD_PRIO_NONE);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_destroy(&attr), 0);
  return 0;
}
