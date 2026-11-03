//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getnetgrent_r
///
//===----------------------------------------------------------------------===//

#include "src/netdb/getnetgrent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/netdb/netgroup_utils.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

// The strings are copied into the caller's buffer. When it is too small the
// triple stays unread, and errno is ERANGE.
LLVM_LIBC_FUNCTION(int, getnetgrent_r,
                   (char **host, char **user, char **domain, char *buf,
                    size_t buflen)) {
  LIBC_CRASH_ON_NULLPTR(host);
  LIBC_CRASH_ON_NULLPTR(user);
  LIBC_CRASH_ON_NULLPTR(domain);
  LIBC_CRASH_ON_NULLPTR(buf);
  netgroup::Group &group = netgroup::current();
  const size_t before = group.cursor;
  netgroup::Triple triple;
  if (!netgroup::next(group, triple))
    return 0;

  const char *fields[3] = {triple.host, triple.user, triple.domain};
  char **out[3] = {host, user, domain};
  size_t used = 0;
  for (int i = 0; i < 3; ++i) {
    if (fields[i] == nullptr) {
      *out[i] = nullptr;
      continue;
    }
    const size_t length = cpp::string_view(fields[i]).size() + 1;
    if (length > buflen - used) {
      group.cursor = before;
      libc_errno = ERANGE;
      return 0;
    }
    inline_memcpy(buf + used, fields[i], length);
    *out[i] = buf + used;
    used += length;
  }
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL
