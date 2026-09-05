//===-- Implementation of sgetspent_r -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/sgetspent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sgetspent_r,
                   (const char *s, struct spwd *spbuf, char *buf, size_t buflen,
                    struct spwd **spbufp)) {
  if (spbufp != nullptr)
    *spbufp = nullptr;
  if (s == nullptr || spbuf == nullptr || buf == nullptr || spbufp == nullptr)
    return EINVAL;

  // The parse works in place, so the line is copied into the caller's
  // buffer first and the entry then points into that.
  size_t len = internal::string_length(s);
  if (len + 1 > buflen)
    return ERANGE;
  for (size_t i = 0; i <= len; ++i)
    buf[i] = s[i];

  // A line read from a file still has its newline; it is not part of the
  // last field.
  if (len > 0 && buf[len - 1] == '\n')
    buf[--len] = '\0';

  if (!internal::parse_line(cpp::span<char>(buf, len + 1), spbuf))
    return EINVAL;

  *spbufp = spbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
