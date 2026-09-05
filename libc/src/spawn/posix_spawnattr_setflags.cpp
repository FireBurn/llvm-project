//===-- Implementation of posix_spawnattr_setflags ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/spawn/posix_spawnattr_setflags.h"

#include "hdr/errno_macros.h"
#include "hdr/spawn_macros.h"
#include "hdr/types/posix_spawnattr_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// These report the error rather than setting errno, which is what every
// call in this family does.
LLVM_LIBC_FUNCTION(int, posix_spawnattr_setflags,
                   (posix_spawnattr_t * attr, short flags)) {
  if (attr == nullptr)
    return EINVAL;
  constexpr short KNOWN = POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP |
                          POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK |
                          POSIX_SPAWN_SETSCHEDPARAM | POSIX_SPAWN_SETSCHEDULER |
                          POSIX_SPAWN_SETSID;
  if ((flags & ~KNOWN) != 0)
    return EINVAL;
  attr->__flags = flags;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
