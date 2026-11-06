//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for pthread_barrierattr_t.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/pthread/pthread_barrierattr_destroy.h"
#include "src/pthread/pthread_barrierattr_getpshared.h"
#include "src/pthread/pthread_barrierattr_init.h"
#include "src/pthread/pthread_barrierattr_setpshared.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

TEST(LlvmLibcPThreadBarrierAttrTest, InitAndDestroy) {
  pthread_barrierattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_destroy(&attr), 0);
}

TEST(LlvmLibcPThreadBarrierAttrTest, DefaultsToPrivate) {
  pthread_barrierattr_t attr;
  // Not a valid value, so that a default which was never written shows.
  int pshared = 42;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, PTHREAD_PROCESS_PRIVATE);
}

TEST(LlvmLibcPThreadBarrierAttrTest, SetsAndReadsBack) {
  pthread_barrierattr_t attr;
  int pshared = 42;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_init(&attr), 0);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_barrierattr_setpshared(&attr,
                                                     PTHREAD_PROCESS_SHARED),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, PTHREAD_PROCESS_SHARED);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_barrierattr_setpshared(&attr,
                                                     PTHREAD_PROCESS_PRIVATE),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, PTHREAD_PROCESS_PRIVATE);
}

TEST(LlvmLibcPThreadBarrierAttrTest, RejectsOtherValues) {
  pthread_barrierattr_t attr;
  int pshared = 42;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_setpshared(&attr, 2), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_setpshared(&attr, -1), EINVAL);
  // A refused value leaves what was there.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_barrierattr_getpshared(&attr, &pshared), 0);
  ASSERT_EQ(pshared, PTHREAD_PROCESS_PRIVATE);
}
