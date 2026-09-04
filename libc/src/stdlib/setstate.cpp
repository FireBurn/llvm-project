//===-- Implementation of setstate ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/setstate.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/random_util.h"

namespace LIBC_NAMESPACE_DECL {

// Goes back to a table an earlier initstate or setstate handed back. The
// table keeps its own place in the sequence, so this picks up where that
// generator left off.
LLVM_LIBC_FUNCTION(char *, setstate, (char *state)) {
  if (state == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  auto &s = random_internal::state;
  random_internal::ensure_seeded();
  random_internal::save(s);
  char *previous = reinterpret_cast<char *>(s.table - 1);

  if (!random_internal::adopt(s, reinterpret_cast<int32_t *>(state))) {
    // The word in front of the table is not one this wrote, so the table is
    // not one it can take up.
    random_internal::adopt(s, reinterpret_cast<int32_t *>(previous));
    libc_errno = EINVAL;
    return nullptr;
  }
  return previous;
}

} // namespace LIBC_NAMESPACE_DECL
