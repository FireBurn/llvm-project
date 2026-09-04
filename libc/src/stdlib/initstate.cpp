//===-- Implementation of initstate ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/initstate.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/random_util.h"

namespace LIBC_NAMESPACE_DECL {

// The size of the table decides how long the sequence runs before it
// repeats. A size which is not one of the ones a table comes in is rounded
// down to the largest that fits.
LLVM_LIBC_FUNCTION(char *, initstate,
                   (unsigned int seed, char *state, size_t size)) {
  if (state == nullptr || size < random_internal::KIND_BYTES[0]) {
    libc_errno = EINVAL;
    return nullptr;
  }

  auto &s = random_internal::state;
  random_internal::ensure_seeded();
  // What the old table was doing is recorded in it before it is let go, so
  // a later setstate can take it up again.
  random_internal::save(s);
  char *previous = reinterpret_cast<char *>(s.table - 1);

  random_internal::configure(s, random_internal::kind_for_size(size));
  s.table = reinterpret_cast<int32_t *>(state) + 1;
  random_internal::seed(s, seed);
  return previous;
}

} // namespace LIBC_NAMESPACE_DECL
