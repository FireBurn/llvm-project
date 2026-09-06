//===-- Implementation of putgrent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/putgrent.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"

namespace LIBC_NAMESPACE_DECL {

// Writes one entry in the form the group file holds it. The member list is
// comma separated, and a group with none has an empty last field.
LLVM_LIBC_FUNCTION(int, putgrent, (const struct group *g, ::FILE *stream)) {
  if (g == nullptr || stream == nullptr || g->gr_name == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  // A name opening with a plus or a minus is one of the lines that used to
  // pull entries in from the network database. Those carry no number of their
  // own, so writing the zero it was read as back out would turn the line into
  // something it was not.
  const bool from_network_database =
      g->gr_name[0] == '+' || g->gr_name[0] == '-';

  const int written =
      from_network_database
          ? LIBC_NAMESPACE::fprintf(stream, "%s:%s::", g->gr_name,
                                    g->gr_passwd != nullptr ? g->gr_passwd : "")
          : LIBC_NAMESPACE::fprintf(stream, "%s:%s:%u:", g->gr_name,
                                    g->gr_passwd != nullptr ? g->gr_passwd : "",
                                    static_cast<unsigned>(g->gr_gid));
  if (written < 0)
    return -1;

  for (size_t i = 0; g->gr_mem != nullptr && g->gr_mem[i] != nullptr; ++i) {
    const char *fmt = i == 0 ? "%s" : ",%s";
    if (LIBC_NAMESPACE::fprintf(stream, fmt, g->gr_mem[i]) < 0)
      return -1;
  }

  if (LIBC_NAMESPACE::fprintf(stream, "\n") < 0)
    return -1;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
