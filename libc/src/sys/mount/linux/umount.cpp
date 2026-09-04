//===-- Linux implementation of umount ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/mount/umount.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/mount/umount2.h"

namespace LIBC_NAMESPACE_DECL {

// umount is umount2 with no flags, which is the whole difference between
// them.
LLVM_LIBC_FUNCTION(int, umount, (const char *target)) {
  return LIBC_NAMESPACE::umount2(target, 0);
}

} // namespace LIBC_NAMESPACE_DECL
