//===-- Implementation of getspent_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/getspent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

// This walks the same shared iteration getspent does, since that is what
// the caller is asking to step through; what it adds is that the entry it
// hands back is in the caller's own buffer.
LLVM_LIBC_FUNCTION(int, getspent_r,
                   (struct spwd * spbuf, char *buf, size_t buflen,
                    struct spwd **spbufp)) {
  if (spbufp != nullptr)
    *spbufp = nullptr;
  if (spbuf == nullptr || buf == nullptr || spbufp == nullptr)
    return EINVAL;

  auto entry = shadow_db::read_next();
  if (!entry.has_value())
    return entry.error();
  if (entry.value() == nullptr)
    return ENOENT;

  // Copy the line the iteration parsed into the caller's buffer and point
  // the caller's entry at that instead.
  const struct spwd *src = entry.value();
  size_t len = 0;
  while (src->sp_namp[len] != '\0')
    ++len;
  size_t pwd_len = 0;
  while (src->sp_pwdp[pwd_len] != '\0')
    ++pwd_len;
  if (len + pwd_len + 2 > buflen)
    return ERANGE;

  char *out = buf;
  for (size_t i = 0; i <= len; ++i)
    out[i] = src->sp_namp[i];
  spbuf->sp_namp = out;
  out += len + 1;
  for (size_t i = 0; i <= pwd_len; ++i)
    out[i] = src->sp_pwdp[i];
  spbuf->sp_pwdp = out;

  spbuf->sp_lstchg = src->sp_lstchg;
  spbuf->sp_min = src->sp_min;
  spbuf->sp_max = src->sp_max;
  spbuf->sp_warn = src->sp_warn;
  spbuf->sp_inact = src->sp_inact;
  spbuf->sp_expire = src->sp_expire;
  spbuf->sp_flag = src->sp_flag;

  *spbufp = spbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
