//===-- Implementation of fsetpos -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fsetpos.h"

#include "hdr/errno_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/fpos_t.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fsetpos, (::FILE * stream, const fpos_t *pos)) {
  if (stream == nullptr || pos == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  auto *file = reinterpret_cast<File *>(stream);
  auto result = file->seek(pos->__pos, SEEK_SET);
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  internal::mbstate state;
  __builtin_memcpy(&state, &pos->__state, sizeof(pos->__state));
  file->set_mbstate(state);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
