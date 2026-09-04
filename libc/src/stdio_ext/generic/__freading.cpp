//===-- Implementation of __freading ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio_ext/__freading.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Whether the last operation on the stream was a read. Code that switches
// direction has to know, because the buffer means something different in
// each case.
LLVM_LIBC_FUNCTION(int, __freading, (::FILE * raw_stream)) {
  if (raw_stream == nullptr)
    return 0;
  auto *stream = reinterpret_cast<LIBC_NAMESPACE::File *>(raw_stream);
  return stream->last_op_was_read() ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL
