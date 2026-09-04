//===-- Implementation of __fpurge --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio_ext/__fpurge.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Throws away whatever is buffered without writing it out. The point is to
// abandon output rather than flush it, so a caller can give up on a stream
// without the data reaching the file.
LLVM_LIBC_FUNCTION(void, __fpurge, (::FILE * raw_stream)) {
  if (raw_stream == nullptr)
    return;
  auto *stream = reinterpret_cast<LIBC_NAMESPACE::File *>(raw_stream);
  stream->discard_buffer();
  return;
}

} // namespace LIBC_NAMESPACE_DECL
