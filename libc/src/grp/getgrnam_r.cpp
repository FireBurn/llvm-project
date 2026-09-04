//===-- Implementation of getgrnam_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/getgrnam_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_r_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

} // anonymous namespace

// The reentrant form: the entry, the names in it and the array of pointers
// to the member names all go in the caller's buffer rather than anywhere
// shared, so two threads may look things up at once and neither disturbs a
// getgrent iteration.
LLVM_LIBC_FUNCTION(int, getgrnam_r,
                   (const char *name, struct group *grp, char *buf,
                    size_t buflen, struct group **result)) {
  if (name == nullptr)
    return EINVAL;
  return group_r::find(grp, buf, buflen, result, [name](struct group *entry) {
    return same(entry->gr_name, name);
  });
}

} // namespace LIBC_NAMESPACE_DECL
