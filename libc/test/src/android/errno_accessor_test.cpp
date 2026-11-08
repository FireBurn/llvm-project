//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __errno.
///
//===----------------------------------------------------------------------===//

#include "src/__support/libc_errno.h"
#include "src/android/__errno.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

TEST(LlvmLibcAndroidErrnoTest, IsTheAddressOfErrno) {
  EXPECT_EQ(LIBC_NAMESPACE::__errno(), &libc_errno);
  libc_errno = 42;
  EXPECT_EQ(*LIBC_NAMESPACE::__errno(), 42);
  *LIBC_NAMESPACE::__errno() = 7;
  EXPECT_EQ(static_cast<int>(libc_errno), 7);
}

#if LIBC_ERRNO_MODE == LIBC_ERRNO_MODE_THREAD_LOCAL
static void *other_thread_errno(void *out) {
  *static_cast<int **>(out) = LIBC_NAMESPACE::__errno();
  return nullptr;
}

TEST(LlvmLibcAndroidErrnoTest, EachThreadHasItsOwn) {
  int *mine = LIBC_NAMESPACE::__errno();
  int *theirs = nullptr;
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, other_thread_errno,
                                           &theirs),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, nullptr), 0);
  EXPECT_TRUE(theirs != nullptr);
  EXPECT_NE(theirs, mine);
}
#endif
