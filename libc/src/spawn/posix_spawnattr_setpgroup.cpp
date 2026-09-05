//===-- Implementation of posix_spawnattr_setpgroup --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/spawn/posix_spawnattr_setpgroup.h"

#include "hdr/errno_macros.h"
#include "hdr/spawn_macros.h"
#include "hdr/types/posix_spawnattr_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// These report the error rather than setting errno, which is what every
// call in this family does.
LLVM_LIBC_FUNCTION(int, posix_spawnattr_setpgroup,
                   (posix_spawnattr_t * attr, pid_t pgroup)) {
  if (attr == nullptr)
    return EINVAL;
  attr->__pgroup = pgroup;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
