//===-- Implementation of __freadahead ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio_ext/__freadahead.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Bytes read into the buffer that the caller has not consumed. Code mixing
// buffered reads with the underlying descriptor needs it to know how far the
// file position has really advanced.
LLVM_LIBC_FUNCTION(size_t, __freadahead, (::FILE * raw_stream)) {
  if (raw_stream == nullptr)
    return 0;
  auto *stream = reinterpret_cast<LIBC_NAMESPACE::File *>(raw_stream);
  return stream->unread_buffered_bytes();
}

} // namespace LIBC_NAMESPACE_DECL
