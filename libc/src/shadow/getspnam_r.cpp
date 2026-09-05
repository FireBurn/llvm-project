//===-- Implementation of getspnam_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/getspnam_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_r_utils.h"

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

LLVM_LIBC_FUNCTION(int, getspnam_r,
                   (const char *name, struct spwd *spbuf, char *buf,
                    size_t buflen, struct spwd **spbufp)) {
  if (name == nullptr)
    return EINVAL;
  return shadow_r::find(spbuf, buf, buflen, spbufp, [name](struct spwd *e) {
    return same(e->sp_namp, name);
  });
}

} // namespace LIBC_NAMESPACE_DECL
