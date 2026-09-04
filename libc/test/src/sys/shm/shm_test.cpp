//===-- Unittests for the System V shared memory calls --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_ipc_macros.h"
#include "hdr/sys_shm_macros.h"
#include "hdr/types/struct_shmid_ds.h"
#include "src/__support/CPP/string_view.h"
#include "src/sys/shm/shmat.h"
#include "src/sys/shm/shmctl.h"
#include "src/sys/shm/shmdt.h"
#include "src/sys/shm/shmget.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcShmTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {
void *const ATTACH_FAILED = reinterpret_cast<void *>(-1);
} // anonymous namespace

TEST_F(LlvmLibcShmTest, CreateAttachAndWrite) {
  int id = LIBC_NAMESPACE::shmget(IPC_PRIVATE, 8192, IPC_CREAT | 0600);
  ASSERT_GE(id, 0);
  ASSERT_ERRNO_SUCCESS();

  char *memory =
      reinterpret_cast<char *>(LIBC_NAMESPACE::shmat(id, nullptr, 0));
  ASSERT_TRUE(memory != ATTACH_FAILED);
  ASSERT_ERRNO_SUCCESS();

  // The segment is real memory, so it can be written and read back.
  const char *message = "shared";
  for (size_t i = 0; i <= 6; ++i)
    memory[i] = message[i];
  EXPECT_TRUE(string_view(memory) == "shared");

  ASSERT_THAT(LIBC_NAMESPACE::shmdt(memory), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::shmctl(id, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcShmTest, ReportsWhatTheSegmentIs) {
  int id = LIBC_NAMESPACE::shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
  ASSERT_GE(id, 0);

  struct shmid_ds info;
  ASSERT_THAT(LIBC_NAMESPACE::shmctl(id, IPC_STAT, &info), Succeeds(0));
  EXPECT_EQ(info.shm_segsz, size_t(4096));
  EXPECT_EQ(info.shm_perm.mode & 0777, mode_t(0600));
  EXPECT_EQ(info.shm_nattch, 0UL);

  void *memory = LIBC_NAMESPACE::shmat(id, nullptr, 0);
  ASSERT_TRUE(memory != ATTACH_FAILED);
  ASSERT_THAT(LIBC_NAMESPACE::shmctl(id, IPC_STAT, &info), Succeeds(0));
  EXPECT_EQ(info.shm_nattch, 1UL);

  ASSERT_THAT(LIBC_NAMESPACE::shmdt(memory), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::shmctl(id, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcShmTest, TwoAttachmentsSeeTheSameMemory) {
  int id = LIBC_NAMESPACE::shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
  ASSERT_GE(id, 0);

  char *first = reinterpret_cast<char *>(LIBC_NAMESPACE::shmat(id, nullptr, 0));
  ASSERT_TRUE(first != ATTACH_FAILED);
  first[0] = 'x';
  first[1] = '\0';

  // A second attachment is a different address onto the same memory.
  char *second =
      reinterpret_cast<char *>(LIBC_NAMESPACE::shmat(id, nullptr, SHM_RDONLY));
  ASSERT_TRUE(second != ATTACH_FAILED);
  EXPECT_TRUE(string_view(second) == "x");

  ASSERT_THAT(LIBC_NAMESPACE::shmdt(second), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::shmdt(first), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::shmctl(id, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcShmTest, BadArguments) {
  struct shmid_ds info;
  EXPECT_THAT(LIBC_NAMESPACE::shmctl(-1, IPC_STAT, &info), Fails(EINVAL, -1));
  // An address which was never attached cannot be detached.
  EXPECT_THAT(LIBC_NAMESPACE::shmdt(reinterpret_cast<void *>(0x1000)),
              Fails(EINVAL, -1));
  EXPECT_TRUE(LIBC_NAMESPACE::shmat(-1, nullptr, 0) == ATTACH_FAILED);
  ASSERT_ERRNO_EQ(EINVAL);
}
