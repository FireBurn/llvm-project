//===-- Implementation of putsgent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/putsgent.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Writes one comma separated list. A null array and an empty one both come
// out as nothing, which is what the file holds for a group with no members.
int print_list(::FILE *stream, char *const *names) {
  if (names == nullptr)
    return 0;
  for (size_t i = 0; names[i] != nullptr; ++i) {
    const char *fmt = i == 0 ? "%s" : ",%s";
    if (LIBC_NAMESPACE::fprintf(stream, fmt, names[i]) < 0)
      return -1;
  }
  return 0;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, putsgent, (const struct sgrp *g, ::FILE *stream)) {
  if (g == nullptr || stream == nullptr || g->sg_namp == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  if (LIBC_NAMESPACE::fprintf(stream, "%s:%s:", g->sg_namp,
                              g->sg_passwd != nullptr ? g->sg_passwd : "") < 0)
    return -1;
  if (print_list(stream, g->sg_adm) < 0)
    return -1;
  if (LIBC_NAMESPACE::fprintf(stream, ":") < 0)
    return -1;
  if (print_list(stream, g->sg_mem) < 0)
    return -1;
  if (LIBC_NAMESPACE::fprintf(stream, "\n") < 0)
    return -1;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
