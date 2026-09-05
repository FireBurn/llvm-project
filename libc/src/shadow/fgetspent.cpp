//===-- Implementation of fgetspent ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/fgetspent.h"

#include "hdr/types/FILE.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/shadow/fgetspent_r.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// The entry the last call returned, which the next one overwrites.
struct spwd entry;
char line[shadow::SHADOW_LINE_SIZE];
} // anonymous namespace

LLVM_LIBC_FUNCTION(struct spwd *, fgetspent, (::FILE * stream)) {
  struct spwd *result = nullptr;
  int err =
      LIBC_NAMESPACE::fgetspent_r(stream, &entry, line, sizeof(line), &result);
  if (err != 0) {
    // Reaching the end of the file is not an error to report.
    if (err != ENOENT)
      libc_errno = err;
    return nullptr;
  }
  return result;
}

} // namespace LIBC_NAMESPACE_DECL
