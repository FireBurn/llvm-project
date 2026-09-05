//===-- Implementation of fgetspent_r -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/fgetspent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"
#include "src/stdio/fgets.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fgetspent_r,
                   (::FILE * stream, struct spwd *spbuf, char *buf,
                    size_t buflen, struct spwd **spbufp)) {
  if (spbufp != nullptr)
    *spbufp = nullptr;
  if (stream == nullptr || spbuf == nullptr || buf == nullptr ||
      spbufp == nullptr)
    return EINVAL;
  if (buflen < 2)
    return ERANGE;

  // Blank lines and comments are skipped, as they are when the database is
  // read as a whole.
  for (;;) {
    if (LIBC_NAMESPACE::fgets(buf, static_cast<int>(buflen), stream) == nullptr)
      return ENOENT;

    size_t len = 0;
    while (buf[len] != '\0')
      ++len;
    if (len > 0 && buf[len - 1] == '\n')
      buf[--len] = '\0';

    if (len == 0 || buf[0] == '#')
      continue;

    if (!internal::parse_line(cpp::span<char>(buf, len + 1), spbuf))
      return EINVAL;
    *spbufp = spbuf;
    return 0;
  }
}

} // namespace LIBC_NAMESPACE_DECL
