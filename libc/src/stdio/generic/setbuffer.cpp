//===-- Implementation of setbuffer ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/setbuffer.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdio/setvbuf.h"

namespace LIBC_NAMESPACE_DECL {

// setbuf with a size of the caller's choosing rather than BUFSIZ. No buffer
// means no buffering, and it cannot report a failure, so one is dropped.
LLVM_LIBC_FUNCTION(void, setbuffer, (::FILE * stream, char *buf, size_t size)) {
  LIBC_NAMESPACE::setvbuf(stream, buf, buf == nullptr ? _IONBF : _IOFBF, size);
}

} // namespace LIBC_NAMESPACE_DECL
