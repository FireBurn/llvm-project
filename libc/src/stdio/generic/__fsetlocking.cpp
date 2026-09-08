//===-- Implementation of __fsetlocking -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/__fsetlocking.h"

#include "hdr/stdio_ext_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Says who takes the stream's lock: the library on every call, or the
// caller. A program which says it will do the locking itself then uses the
// _unlocked calls, which is the point of the whole thing.
LLVM_LIBC_FUNCTION(int, __fsetlocking, (::FILE * stream, int type)) {
  auto *file = reinterpret_cast<LIBC_NAMESPACE::File *>(stream);
  if (file == nullptr)
    return FSETLOCKING_INTERNAL;

  const int current =
      file->has_caller_locking() ? FSETLOCKING_BYCALLER : FSETLOCKING_INTERNAL;

  if (type == FSETLOCKING_BYCALLER)
    file->set_caller_locking(true);
  else if (type == FSETLOCKING_INTERNAL)
    file->set_caller_locking(false);
  // FSETLOCKING_QUERY, and anything else, only asks.

  return current;
}

} // namespace LIBC_NAMESPACE_DECL
