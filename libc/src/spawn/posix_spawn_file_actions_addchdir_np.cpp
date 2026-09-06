//===-- Implementation of posix_spawn_file_actions_addchdir_np ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "posix_spawn_file_actions_addchdir_np.h"

#include "file_actions.h"
#include "src/__support/CPP/new.h"
#include "src/__support/alloc-checker.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <spawn.h>

namespace LIBC_NAMESPACE_DECL {

// Says where the child is to start, so that a caller wanting it in a
// directory of its own still has one call which does the whole thing. The
// path is not copied: the caller has to keep it around until the spawn, which
// is what it has to do for the paths the other actions name too.
LLVM_LIBC_FUNCTION(int, posix_spawn_file_actions_addchdir_np,
                   (posix_spawn_file_actions_t *__restrict actions,
                    const char *__restrict path)) {
  if (actions == nullptr || path == nullptr)
    return EINVAL;

  AllocChecker ac;
  auto *act = new (ac) SpawnFileChdirAction(path);
  if (act == nullptr)
    return ENOMEM;
  BaseSpawnFileAction::add_action(actions, act);

  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
