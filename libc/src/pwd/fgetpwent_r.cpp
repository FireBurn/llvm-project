//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fgetpwent_r
///
//===----------------------------------------------------------------------===//

#include "src/pwd/fgetpwent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/pwd/pwd_utils.h"
#include "src/stdio/fgets.h"

namespace LIBC_NAMESPACE_DECL {

// The entry is parsed in place, so its strings live in the caller's buffer.
LLVM_LIBC_FUNCTION(int, fgetpwent_r,
                   (::FILE * stream, struct passwd *pwbuf, char *buf,
                    size_t buflen, struct passwd **pwbufp)) {
  LIBC_CRASH_ON_NULLPTR(stream);
  LIBC_CRASH_ON_NULLPTR(pwbuf);
  LIBC_CRASH_ON_NULLPTR(buf);
  LIBC_CRASH_ON_NULLPTR(pwbufp);
  *pwbufp = nullptr;

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

    auto parsed = pwd::parse_line<struct passwd>(cpp::span<char>(buf, len + 1),
                                                 cpp::span<char>(), pwbuf);
    if (!parsed.has_value())
      return parsed.error();
    *pwbufp = pwbuf;
    return 0;
  }
}

} // namespace LIBC_NAMESPACE_DECL
