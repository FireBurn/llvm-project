//===-- Implementation of __fseterr ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio_ext/__fseterr.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Sets the stream's error indicator. Code that wraps a stream needs it to
// report a failure of its own through the stream the caller is holding, and
// there is no standard way to do it.
LLVM_LIBC_FUNCTION(void, __fseterr, (::FILE * raw_stream)) {
  if (raw_stream == nullptr)
    return;
  auto *stream = reinterpret_cast<LIBC_NAMESPACE::File *>(raw_stream);
  stream->set_error();
}

} // namespace LIBC_NAMESPACE_DECL
