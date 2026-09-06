//===-- Implementation of getsgent_r ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/getsgent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

// The iteration this walks is the one every caller shares, so this is
// reentrant only in that the record is copied out rather than left in the
// library. glibc's is the same.
LLVM_LIBC_FUNCTION(int, getsgent_r,
                   (struct sgrp * sgbuf, char *buf, size_t buflen,
                    struct sgrp **sgbufp)) {
  if (sgbufp != nullptr)
    *sgbufp = nullptr;
  if (sgbuf == nullptr || buf == nullptr || sgbufp == nullptr)
    return EINVAL;
  (void)buflen;

  auto entry = gshadow_db::read_next();
  if (!entry.has_value())
    return entry.error();
  if (entry.value() == nullptr)
    return ENOENT; // The end of the file.

  inline_memcpy(sgbuf, entry.value(), sizeof(*sgbuf));
  *sgbufp = sgbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
