//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fgetgrent_r
///
//===----------------------------------------------------------------------===//

#include "src/grp/fgetgrent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/grp/grp_utils.h"
#include "src/stdio/fgets.h"

namespace LIBC_NAMESPACE_DECL {

// The entry is parsed in place. The array gr_mem points at is built in the
// rest of the buffer, after the line.
LLVM_LIBC_FUNCTION(int, fgetgrent_r,
                   (::FILE * stream, struct group *grbuf, char *buf,
                    size_t buflen, struct group **grbufp)) {
  LIBC_CRASH_ON_NULLPTR(stream);
  LIBC_CRASH_ON_NULLPTR(grbuf);
  LIBC_CRASH_ON_NULLPTR(buf);
  LIBC_CRASH_ON_NULLPTR(grbufp);
  *grbufp = nullptr;

  for (;;) {
    if (buflen < 2)
      return ERANGE;
    if (internal::fgets_impl(buf, static_cast<int>(buflen), stream) == nullptr)
      return ENOENT;

    size_t len = 0;
    while (buf[len] != '\0')
      ++len;
    if (len > 0 && buf[len - 1] == '\n')
      buf[--len] = '\0';
    else if (len + 1 == buflen)
      return ERANGE; // The line does not fit.

    if (len == 0 || buf[0] == '#')
      continue;

    size_t used = len + 1;
    size_t skew = (reinterpret_cast<size_t>(buf) + used) % alignof(char *);
    if (skew != 0)
      used += alignof(char *) - skew;
    if (used > buflen)
      return ERANGE;
    auto parsed = pwd::parse_line<struct group>(
        cpp::span<char>(buf, len + 1),
        cpp::span<char>(buf + used, buflen - used), grbuf);
    if (!parsed.has_value())
      return parsed.error();
    *grbufp = grbuf;
    return 0;
  }
}

} // namespace LIBC_NAMESPACE_DECL
