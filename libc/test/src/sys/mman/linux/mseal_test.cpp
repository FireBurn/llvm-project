//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of mseal and process_madvise.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/types/struct_iovec.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/mman/mprotect.h"
#include "src/sys/mman/mseal.h"
#include "src/sys/mman/munmap.h"
#include "src/sys/mman/process_madvise.h"
#include "src/sys/pidfd/pidfd_open.h"
#include "src/unistd/close.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcMsealTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcMsealTest, SealedMappingRefusesChanges) {
  constexpr size_t SIZE = 4096;
  void *addr = LIBC_NAMESPACE::mmap(nullptr, SIZE, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NE(addr, MAP_FAILED);

  // Kernels before 6.10 do not have the call, and have nothing to seal with.
  if (LIBC_NAMESPACE::mseal(addr, SIZE, 0) == -1 && libc_errno == ENOSYS) {
    libc_errno = 0;
    ASSERT_EQ(LIBC_NAMESPACE::munmap(addr, SIZE), 0);
    return;
  }
  ASSERT_THAT(LIBC_NAMESPACE::mprotect(addr, SIZE, PROT_READ), Fails(EPERM));
  ASSERT_THAT(LIBC_NAMESPACE::munmap(addr, SIZE), Fails(EPERM));
}

TEST_F(LlvmLibcMsealTest, RefusesUnknownFlags) {
  constexpr size_t SIZE = 4096;
  void *addr = LIBC_NAMESPACE::mmap(nullptr, SIZE, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NE(addr, MAP_FAILED);
  int ret = LIBC_NAMESPACE::mseal(addr, SIZE, 1);
  ASSERT_EQ(ret, -1);
  ASSERT_TRUE(libc_errno == EINVAL || libc_errno == ENOSYS);
  libc_errno = 0;
  ASSERT_EQ(LIBC_NAMESPACE::munmap(addr, SIZE), 0);
}

TEST_F(LlvmLibcMsealTest, ProcessMadviseAdvisesAnotherProcessesRange) {
  constexpr size_t SIZE = 4096;
  void *addr = LIBC_NAMESPACE::mmap(nullptr, SIZE, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NE(addr, MAP_FAILED);
  int pidfd = LIBC_NAMESPACE::pidfd_open(LIBC_NAMESPACE::getpid(), 0);
  ASSERT_GE(pidfd, 0);

  struct iovec range = {addr, SIZE};
  ssize_t ret = LIBC_NAMESPACE::process_madvise(pidfd, &range, 1, MADV_COLD, 0);
  if (ret == -1 && libc_errno == ENOSYS) {
    libc_errno = 0;
  } else {
    ASSERT_EQ(ret, ssize_t(SIZE));
  }
  // An advice the call does not take is refused.
  ASSERT_EQ(LIBC_NAMESPACE::process_madvise(pidfd, &range, 1, -1, 0),
            ssize_t(-1));
  libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::close(pidfd), 0);
  ASSERT_EQ(LIBC_NAMESPACE::munmap(addr, SIZE), 0);
}
