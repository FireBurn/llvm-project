//===-- Implementation of __fpending --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio_ext/__fpending.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// How many bytes have been written to the stream but not yet handed to the
// system. Programs use it to tell whether a close that failed lost data.
LLVM_LIBC_FUNCTION(size_t, __fpending, (::FILE * raw_stream)) {
  if (raw_stream == nullptr)
    return 0;
  auto *stream = reinterpret_cast<LIBC_NAMESPACE::File *>(raw_stream);
  return stream->pending_write_bytes();
}

} // namespace LIBC_NAMESPACE_DECL
