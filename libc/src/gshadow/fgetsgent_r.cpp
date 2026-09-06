//===-- Implementation of fgetsgent_r ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/fgetsgent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_r_utils.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/stdio/fgets.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fgetsgent_r,
                   (::FILE * stream, struct sgrp *sgbuf, char *buf,
                    size_t buflen, struct sgrp **sgbufp)) {
  if (sgbufp != nullptr)
    *sgbufp = nullptr;
  if (stream == nullptr || sgbuf == nullptr || buf == nullptr ||
      sgbufp == nullptr)
    return EINVAL;

  gshadow_r::Split split;
  if (!gshadow_r::split_buffer(buf, buflen, &split))
    return ERANGE;

  // Blank lines and comments are passed over.
  for (;;) {
    if (LIBC_NAMESPACE::fgets(split.line.data(),
                              static_cast<int>(split.line.size()),
                              stream) == nullptr)
      return ENOENT; // The end of the file, which is not an error to report.

    size_t len = 0;
    while (split.line[len] != '\0')
      ++len;
    if (len > 0 && split.line[len - 1] == '\n')
      split.line[--len] = '\0';

    if (len == 0 || split.line[0] == '#')
      continue;

    if (!gshadow::parse_line_into(split.line.first(len + 1), sgbuf,
                                  split.admins, split.max_admins, split.members,
                                  split.max_members))
      return EINVAL;

    *sgbufp = sgbuf;
    return 0;
  }
}

} // namespace LIBC_NAMESPACE_DECL
