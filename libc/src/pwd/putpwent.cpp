//===-- Implementation of putpwent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/putpwent.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
const char *or_empty(const char *s) { return s != nullptr ? s : ""; }
} // anonymous namespace

// Writes one entry in the form the password file holds it.
LLVM_LIBC_FUNCTION(int, putpwent, (const struct passwd *p, ::FILE *stream)) {
  if (p == nullptr || stream == nullptr || p->pw_name == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  // A name opening with a plus or a minus is one of the lines that used to
  // pull entries in from the network database. Those carry no numbers of
  // their own, so writing the zeroes they were read as back out would turn
  // the line into something it was not.
  const bool from_network_database =
      p->pw_name[0] == '+' || p->pw_name[0] == '-';

  int written;
  if (from_network_database)
    written = LIBC_NAMESPACE::fprintf(
        stream, "%s:%s:::%s:%s:%s\n", p->pw_name, or_empty(p->pw_passwd),
        or_empty(p->pw_gecos), or_empty(p->pw_dir), or_empty(p->pw_shell));
  else
    written = LIBC_NAMESPACE::fprintf(
        stream, "%s:%s:%u:%u:%s:%s:%s\n", p->pw_name, or_empty(p->pw_passwd),
        static_cast<unsigned>(p->pw_uid), static_cast<unsigned>(p->pw_gid),
        or_empty(p->pw_gecos), or_empty(p->pw_dir), or_empty(p->pw_shell));

  if (written < 0)
    return -1;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
