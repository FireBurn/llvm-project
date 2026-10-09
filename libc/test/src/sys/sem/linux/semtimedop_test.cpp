//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for semtimedop
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_ipc_macros.h"
#include "hdr/sys_sem_macros.h"
#include "hdr/types/struct_sembuf.h"
#include "hdr/types/struct_semid_ds.h"
#include "hdr/types/struct_timespec.h"
#include "src/sys/sem/semctl.h"
#include "src/sys/sem/semget.h"
#include "src/sys/sem/semtimedop.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcSemtimedopTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSemtimedopTest, WaitsForAValueThatNeverComesAndGivesUp) {
  int semid =
      LIBC_NAMESPACE::semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(semid, -1);

  // The semaphore is at zero, so taking one blocks until the deadline.
  struct sembuf take = {0, -1, 0};
  struct timespec timeout = {0, 10 * 1000 * 1000};
  ASSERT_THAT(LIBC_NAMESPACE::semtimedop(semid, &take, 1, &timeout),
              Fails(EAGAIN));

  ASSERT_THAT(LIBC_NAMESPACE::semctl(semid, 0, IPC_RMID), Succeeds(0));
}

TEST_F(LlvmLibcSemtimedopTest, ReturnsAtOnceWhenTheOperationCanProceed) {
  int semid =
      LIBC_NAMESPACE::semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
  ASSERT_GT(semid, -1);

  struct sembuf give = {0, 1, 0};
  struct timespec timeout = {5, 0};
  ASSERT_THAT(LIBC_NAMESPACE::semtimedop(semid, &give, 1, &timeout),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::semctl(semid, 0, GETVAL), Succeeds(1));

  ASSERT_THAT(LIBC_NAMESPACE::semctl(semid, 0, IPC_RMID), Succeeds(0));
}

TEST_F(LlvmLibcSemtimedopTest, WithoutATimeoutItIsSemop) {
  int semid =
      LIBC_NAMESPACE::semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
  ASSERT_GT(semid, -1);

  struct sembuf give = {0, 2, 0};
  ASSERT_THAT(LIBC_NAMESPACE::semtimedop(semid, &give, 1, nullptr),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::semctl(semid, 0, GETVAL), Succeeds(2));

  ASSERT_THAT(LIBC_NAMESPACE::semctl(semid, 0, IPC_RMID), Succeeds(0));
}
