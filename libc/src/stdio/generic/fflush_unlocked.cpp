//===-- Implementation of fflush_unlocked ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fflush_unlocked.h"
#include "src/__support/File/file.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The caller holds the lock, which is what the name promises, so unlike
// fflush this takes no stream but the one it was given: flushing every open
// stream would need each of their locks.
LLVM_LIBC_FUNCTION(int, fflush_unlocked, (::FILE * stream)) {
  int result =
      reinterpret_cast<LIBC_NAMESPACE::File *>(stream)->flush_unlocked();
  if (result != 0) {
    libc_errno = result;
    return EOF;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
