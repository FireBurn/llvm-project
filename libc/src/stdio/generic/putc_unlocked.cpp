//===-- Implementation of putc_unlocked -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/putc_unlocked.h"
#include "src/__support/File/file.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, putc_unlocked, (int c, ::FILE *stream)) {
  unsigned char uc = static_cast<unsigned char>(c);

  auto result =
      reinterpret_cast<LIBC_NAMESPACE::File *>(stream)->write_unlocked(&uc, 1);
  if (result.has_error())
    libc_errno = result.error;

  if (result.value != 1)
    return EOF;
  return static_cast<int>(uc);
}

} // namespace LIBC_NAMESPACE_DECL
