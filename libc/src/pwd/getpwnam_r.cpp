//===-- Implementation of getpwnam_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/getpwnam_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pwd/pwd_r_utils.h"
#include "src/string/string_utils.h"

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

// The reentrant form: the entry and the strings in it go where the caller
// asked rather than into anything shared, so two threads looking things up
// at once do not tread on each other, and neither disturbs a getpwent
// iteration.
LLVM_LIBC_FUNCTION(int, getpwnam_r,
                   (const char *name, struct passwd *pwd, char *buf,
                    size_t buflen, struct passwd **result)) {
  if (name == nullptr)
    return EINVAL;
  return passwd_r::find(pwd, buf, buflen, result, [name](struct passwd *entry) {
    return same(entry->pw_name, name);
  });
}

} // namespace LIBC_NAMESPACE_DECL
