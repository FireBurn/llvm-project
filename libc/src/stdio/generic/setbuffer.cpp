//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of setbuffer
///
//===----------------------------------------------------------------------===//

#include "src/stdio/setbuffer.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// setbuf with a size of the caller's choosing rather than BUFSIZ. No buffer
// means no buffering, and it cannot report a failure, so one is dropped.
LLVM_LIBC_FUNCTION(void, setbuffer, (::FILE * stream, char *buf, size_t size)) {
  reinterpret_cast<LIBC_NAMESPACE::File *>(stream)->set_buffer(
      buf, size, buf == nullptr ? _IONBF : _IOFBF);
}

} // namespace LIBC_NAMESPACE_DECL
