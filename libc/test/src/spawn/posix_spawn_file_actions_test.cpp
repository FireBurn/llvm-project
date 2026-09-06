//===-- Unittests for posix_spwan_file_actions_t manipulation -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/spawn/file_actions.h"
#include "src/spawn/posix_spawn_file_actions_addchdir_np.h"
#include "src/spawn/posix_spawn_file_actions_addclose.h"
#include "src/spawn/posix_spawn_file_actions_adddup2.h"
#include "src/spawn/posix_spawn_file_actions_addfchdir_np.h"
#include "src/spawn/posix_spawn_file_actions_addopen.h"
#include "src/spawn/posix_spawn_file_actions_destroy.h"
#include "src/spawn/posix_spawn_file_actions_init.h"
#include "test/UnitTest/Test.h"

#include <spawn.h>

TEST(LlvmLibcPosixSpawnFileActionsTest, AddActions) {
  posix_spawn_file_actions_t actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&actions), 0);

  ASSERT_EQ(uintptr_t(actions.__front), uintptr_t(nullptr));
  ASSERT_EQ(uintptr_t(actions.__back), uintptr_t(nullptr));

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addclose(&actions, 10), 0);
  ASSERT_NE(uintptr_t(actions.__front), uintptr_t(nullptr));
  ASSERT_NE(uintptr_t(actions.__back), uintptr_t(nullptr));

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_adddup2(&actions, 11, 12),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addopen(
                &actions, 13, "path/to/file", 0, 0),
            0);

  LIBC_NAMESPACE::BaseSpawnFileAction *act =
      reinterpret_cast<LIBC_NAMESPACE::BaseSpawnFileAction *>(actions.__front);
  int action_count = 0;
  while (act != nullptr) {
    ++action_count;
    if (action_count == 1) {
      ASSERT_EQ(act->type, LIBC_NAMESPACE::BaseSpawnFileAction::CLOSE);
    }
    if (action_count == 2) {
      ASSERT_EQ(act->type, LIBC_NAMESPACE::BaseSpawnFileAction::DUP2);
    }
    if (action_count == 3) {
      ASSERT_EQ(act->type, LIBC_NAMESPACE::BaseSpawnFileAction::OPEN);
    }
    act = act->next;
  }
  ASSERT_EQ(action_count, 3);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&actions), 0);
}

TEST(LlvmLibcPosixSpawnFileActionsTest, InvalidActions) {
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addclose(nullptr, 1),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_adddup2(nullptr, 1, 2),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addopen(nullptr, 1,
                                                             nullptr, 0, 0),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(nullptr), EINVAL);

  posix_spawn_file_actions_t actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&actions), 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addclose(&actions, -1),
            EBADF);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_adddup2(&actions, -1, 2),
            EBADF);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_adddup2(&actions, 1, -2),
            EBADF);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addopen(&actions, -1,
                                                             nullptr, 0, 0),
            EBADF);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&actions), 0);
}

// The child can be told to change directory before it runs, either by name
// or by a descriptor it already holds.
TEST(LlvmLibcPosixSpawnFileActionsTest, ChangingDirectory) {
  posix_spawn_file_actions_t actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&actions), 0);

  ASSERT_EQ(
      LIBC_NAMESPACE::posix_spawn_file_actions_addchdir_np(&actions, "/tmp"),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addfchdir_np(&actions, 1),
            0);

  int action_count = 0;
  auto *act =
      reinterpret_cast<LIBC_NAMESPACE::BaseSpawnFileAction *>(actions.__front);
  while (act != nullptr) {
    ++action_count;
    if (action_count == 1)
      ASSERT_EQ(int(act->type),
                int(LIBC_NAMESPACE::BaseSpawnFileAction::CHDIR));
    if (action_count == 2)
      ASSERT_EQ(int(act->type),
                int(LIBC_NAMESPACE::BaseSpawnFileAction::FCHDIR));
    act = act->next;
  }
  ASSERT_EQ(action_count, 2);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&actions), 0);
}

TEST(LlvmLibcPosixSpawnFileActionsTest, InvalidDirectoryActions) {
  ASSERT_EQ(
      LIBC_NAMESPACE::posix_spawn_file_actions_addchdir_np(nullptr, "/tmp"),
      EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addfchdir_np(nullptr, 1),
            EINVAL);

  posix_spawn_file_actions_t actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&actions), 0);
  ASSERT_EQ(
      LIBC_NAMESPACE::posix_spawn_file_actions_addchdir_np(&actions, nullptr),
      EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_addfchdir_np(&actions, -1),
            EBADF);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&actions), 0);
}
