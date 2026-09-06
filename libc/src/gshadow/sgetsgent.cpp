//===-- Implementation of sgetsgent -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/sgetsgent.h"

#include "hdr/types/struct_sgrp.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/gshadow/sgetsgent_r.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// The entry the last call returned, which the next one overwrites.
struct sgrp entry;
char line[gshadow::GSHADOW_LINE_SIZE];
} // anonymous namespace

LLVM_LIBC_FUNCTION(struct sgrp *, sgetsgent, (const char *s)) {
  struct sgrp *result = nullptr;
  int err = LIBC_NAMESPACE::sgetsgent_r(s, &entry, line, sizeof(line), &result);
  if (err != 0) {
    libc_errno = err;
    return nullptr;
  }
  return result;
}

} // namespace LIBC_NAMESPACE_DECL
