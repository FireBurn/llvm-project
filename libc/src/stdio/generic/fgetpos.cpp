//===-- Implementation of fgetpos -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fgetpos.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/fpos_t.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fgetpos,
                   (::FILE *__restrict stream, fpos_t *__restrict pos)) {
  if (stream == nullptr || pos == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  auto *file = reinterpret_cast<File *>(stream);
  auto result = file->tell();
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  pos->__pos = result.value();
  // The conversion state goes with the offset, so a wide oriented stream can
  // be put back exactly where it was.
  auto state = file->get_mbstate();
  __builtin_memcpy(&pos->__state, &state, sizeof(pos->__state));
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
