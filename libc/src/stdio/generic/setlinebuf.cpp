//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of setlinebuf
///
//===----------------------------------------------------------------------===//

#include "src/stdio/setlinebuf.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The older spelling of asking for line buffering. It cannot report a
// failure, so one is dropped. A null buffer with no size leaves the stream
// without any buffer to hold a line in, so it asks for the usual size.
LLVM_LIBC_FUNCTION(void, setlinebuf, (::FILE * stream)) {
  reinterpret_cast<LIBC_NAMESPACE::File *>(stream)->set_buffer(nullptr, BUFSIZ,
                                                               _IOLBF);
}

} // namespace LIBC_NAMESPACE_DECL
