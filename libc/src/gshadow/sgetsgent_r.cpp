//===-- Implementation of sgetsgent_r ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/sgetsgent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_r_utils.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sgetsgent_r,
                   (const char *s, struct sgrp *sgbuf, char *buf, size_t buflen,
                    struct sgrp **sgbufp)) {
  if (sgbufp != nullptr)
    *sgbufp = nullptr;
  if (s == nullptr || sgbuf == nullptr || buf == nullptr || sgbufp == nullptr)
    return EINVAL;

  gshadow_r::Split split;
  if (!gshadow_r::split_buffer(buf, buflen, &split))
    return ERANGE;

  // The parse works in place and the entry points into the buffer, so the
  // line is copied there first.
  size_t len = internal::string_length(s);
  if (len + 1 > split.line.size())
    return ERANGE;
  for (size_t i = 0; i <= len; ++i)
    split.line[i] = s[i];

  // A line read from a file still has its newline; it is not part of the
  // last field.
  if (len > 0 && split.line[len - 1] == '\n')
    split.line[--len] = '\0';

  if (!gshadow::parse_line_into(split.line.first(len + 1), sgbuf, split.admins,
                                split.max_admins, split.members,
                                split.max_members))
    return EINVAL;

  *sgbufp = sgbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
