//===-- Unittests for the limits macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the limits macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/limits-macros.h"

TEST(LlvmLibcLimitsMacrosTest, AddNgroups_MaxAnd_Posix_Ngroups_Max) {
  EXPECT_EQ(static_cast<long long>(_POSIX_NGROUPS_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(NGROUPS_MAX), 0x10000LL);
}

TEST(LlvmLibcLimitsMacrosTest, FillInThePosixLimitsHValues) {
  EXPECT_EQ(static_cast<long long>(MAX_CANON), 255LL);
  EXPECT_EQ(static_cast<long long>(MAX_INPUT), 255LL);
  EXPECT_EQ(static_cast<long long>(PIPE_BUF), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(LOGIN_NAME_MAX), 256LL);
  EXPECT_EQ(static_cast<long long>(TTY_NAME_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(MQ_PRIO_MAX), 0x8000LL);
  EXPECT_EQ(static_cast<long long>(RTSIG_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(DELAYTIMER_MAX), 0x7fffffffLL);
  EXPECT_EQ(static_cast<long long>(AIO_PRIO_DELTA_MAX), 20LL);
  EXPECT_EQ(static_cast<long long>(PTHREAD_KEYS_MAX), 1024LL);
  EXPECT_EQ(static_cast<long long>(PTHREAD_STACK_MIN), 0x4000LL);
  EXPECT_EQ(static_cast<long long>(XATTR_NAME_MAX), 255LL);
  EXPECT_EQ(static_cast<long long>(XATTR_SIZE_MAX), 0x10000LL);
  EXPECT_EQ(static_cast<long long>(XATTR_LIST_MAX), 0x10000LL);
  EXPECT_EQ(static_cast<long long>(CHARCLASS_NAME_MAX), 2048LL);
  EXPECT_EQ(static_cast<long long>(RE_DUP_MAX), 255LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_AIO_LISTIO_MAX), 2LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_AIO_MAX), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_CHILD_MAX), 25LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_CLOCKRES_MIN), 0x1312d00LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_DELAYTIMER_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_LINK_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_LOGIN_NAME_MAX), 9LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MAX_CANON), 255LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MQ_OPEN_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MQ_PRIO_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_OPEN_MAX), 20LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_PIPE_BUF), 512LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_RE_DUP_MAX), 255LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_RTSIG_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SEM_NSEMS_MAX), 256LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SEM_VALUE_MAX), 0x7fffLL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SIGQUEUE_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SSIZE_MAX), 0x7fffLL);
  EXPECT_EQ(static_cast<long long>(_POSIX_STREAM_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SYMLINK_MAX), 255LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SYMLOOP_MAX), 8LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_THREAD_KEYS_MAX), 128LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_THREAD_THREADS_MAX), 64LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_TIMER_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_TTY_NAME_MAX), 9LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_TZNAME_MAX), 6LL);
}

TEST(LlvmLibcLimitsMacrosTest, Posix2UtilityLimits) {
  EXPECT_EQ(static_cast<long long>(_POSIX2_BC_BASE_MAX), 99LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_BC_DIM_MAX), 2048LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_BC_SCALE_MAX), 99LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_BC_STRING_MAX), 1000LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_CHARCLASS_NAME_MAX), 14LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_COLL_WEIGHTS_MAX), 2LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_EXPR_NEST_MAX), 32LL);
  EXPECT_EQ(static_cast<long long>(_POSIX2_RE_DUP_MAX), 255LL);
}
