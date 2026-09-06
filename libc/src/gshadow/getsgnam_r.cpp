//===-- Implementation of getsgnam_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/getsgnam_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_r_utils.h"
#include "src/gshadow/gshadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

} // anonymous namespace

// This opens the file for itself rather than sharing the handle the
// iteration uses, so that it can be called from any thread.
LLVM_LIBC_FUNCTION(int, getsgnam_r,
                   (const char *name, struct sgrp *sgbuf, char *buf,
                    size_t buflen, struct sgrp **sgbufp)) {
  if (sgbufp != nullptr)
    *sgbufp = nullptr;
  if (name == nullptr || sgbuf == nullptr || buf == nullptr ||
      sgbufp == nullptr)
    return EINVAL;

  gshadow_r::Split split;
  if (!gshadow_r::split_buffer(buf, buflen, &split))
    return ERANGE;

  internal::FlatFileDatabase<struct sgrp> db(gshadow_db::path());
  if (auto opened = db.setdb(); !opened.has_value())
    return opened.error();

  for (;;) {
    auto read = db.getline(split.line);
    if (!read.has_value()) {
      db.enddb();
      return read.error();
    }
    if (!read.value())
      break; // The end of the file, and no match is not an error.

    if (!gshadow::parse_line_into(split.line.first(*read.value() + 1), sgbuf,
                                  split.admins, split.max_admins, split.members,
                                  split.max_members))
      continue; // A line which will not parse is passed over.

    if (same(sgbuf->sg_namp, name)) {
      db.enddb();
      *sgbufp = sgbuf;
      return 0;
    }
  }
  db.enddb();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
